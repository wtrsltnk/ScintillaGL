#include "stringhelpers.hpp"

#include <regex>

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
void ltrim(std::string &s)
{
    s.erase(
        s.begin(),
        std::find_if(
            s.begin(),
            s.end(),
            [](unsigned char ch) {
                return !std::isspace(ch) && ch != 0;
            }));
}

// trim comment from start (in place)
void trimComment(std::string &s)
{
    s.erase(
        s.begin(),
        std::find_if(
            s.begin(),
            s.end(),
            [](unsigned char ch) {
                return !std::isspace(ch) && ch != 0 && ch != '-' && ch != '/';
            }));
}

// trim from end (in place)
void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch) && ch != 0;
            }).base(),
            s.end());
}

// trim from both ends (in place)
void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s)
{
    trim(s);
    return s;
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

std::map<std::string, std::string> ParseHeaders(
    const std::vector<std::string> &lines,
    size_t &headersEndAt)
{
    std::map<std::string, std::string> headers;
    std::regex r("([\\w-]+): (.+)", std::regex::icase);

    headersEndAt = 0;
    for (size_t i = 0; i < lines.size(); i++)
    {
        if (lines[i].empty() || (lines[i][0] != '-' && lines[i][1] != '-'))
        {
            headersEndAt = i + 1;
            break;
        }

        auto line = lines[i];
        trimComment(line);

        std::smatch match;
        if (!std::regex_search(lines[i], match, r))
        {
            headersEndAt = i;
            break;
        }

        headers.insert(std::make_pair(match.str(1), match.str(2)));
    }

    return headers;
}
