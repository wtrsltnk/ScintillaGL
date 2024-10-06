#include "filerunnerservice.hpp"

#include <algorithm> // std::equal
#include <cctype>    // std::tolower
#include <filesystem>
#include <httpclient.hpp>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <stringcontent.hpp>
#include <vector>

bool ichar_equals(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

bool iequals(const std::string &a, const std::string &b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), ichar_equals);
}

// trim from start (in place)
inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch) && ch != 0;
            }));
}

// trim from end (in place)
inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch) && ch != 0;
            }).base(),
            s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}

FileRunnerService::FileRunnerService() {}

std::string FileRunnerService::Execute(
    const std::string &title,
    const std::string &content)
{
    auto ext = std::filesystem::path(title).extension();
    std::cout << ext << std::endl;

    if (ext == ".http")
    {
        return ExecuteHttp(content);
    }

    return "Unsupported file extension";
}

std::string Return(
    const std::shared_ptr<HttpResponseMessage> response)
{
    auto content = response->Content->ReadAsString();
    trim(content);
    return content;
}

std::vector<std::string> split_string(
    const std::string &str,
    const std::string &delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

std::string FileRunnerService::ExecuteHttp(
    const std::string &content)
{
    auto lines = split_string(content, "\n");

    if (lines.empty())
    {
        return "// ERR: Could not determine HTTP method ";
    }

    HttpClient client;

    auto firstLine = lines.front();
    lines.erase(lines.begin());

    auto firstSpace = ltrim_copy(firstLine).find_first_of(' ');
    if (firstSpace == std::string::npos)
    {
        return "// ERR: Could not determine HTTP method and URL ";
    }

    auto url = firstLine.substr(firstSpace);
    trim(url);

    if (iequals(firstLine.substr(0, 4), "post"))
    {
        return Return(client.Post(url, ParseRequestContent(lines)));
    }

    if (iequals(firstLine.substr(0, 4), "put"))
    {
        return Return(client.Put(url, ParseRequestContent(lines)));
    }

    if (iequals(firstLine.substr(0, 4), "patch"))
    {
        return Return(client.Patch(url, ParseRequestContent(lines)));
    }

    if (iequals(firstLine.substr(0, 4), "delete"))
    {
        return Return(client.Delete(url));
    }

    return Return(client.Get(url));
}

std::shared_ptr<HttpContent> FileRunnerService::ParseRequestContent(
    const std::vector<std::string> &lines)
{
    std::map<std::string, std::string> headers;
    std::regex r("([\\w-]+): (.+)", std::regex::icase);

    size_t headersEndAt = 0;
    for (size_t i = 0; i < lines.size(); i++)
    {
        if (lines[i].empty())
        {
            headersEndAt = i;
            break;
        }

        std::smatch match;
        if (!std::regex_search(lines[i], match, r))
        {
            headersEndAt = i;
            break;
        }

        headers.insert(std::make_pair(match.str(1), match.str(2)));
    }

    std::ostringstream imploded;
    std::copy(lines.begin() + headersEndAt, lines.end(),
              std::ostream_iterator<std::string>(imploded, "\n"));

    auto httpContent = std::make_shared<StringContent>(imploded.str());

    return httpContent;
}
