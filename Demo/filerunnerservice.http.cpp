#include "filerunnerservice.hpp"

#include "stringhelpers.hpp"
#include <algorithm> // std::equal
#include <httpclient.hpp>
#include <iterator>
#include <sstream>
#include <string>
#include <stringcontent.hpp>
#include <vector>

std::string Return(
    const std::shared_ptr<HttpResponseMessage> response)
{
    auto content = response->Content->ReadAsString();
    trim(content);
    return content;
}

std::string FileRunnerService::ExecuteHttp(
    const std::string &firstLine,
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    HttpClient client;

    auto firstSpace = ltrim_copy(firstLine).find_first_of(' ');
    if (firstSpace == std::string::npos)
    {
        return "// ERR: Could not determine HTTP method and URL ";
    }

    auto url = firstLine.substr(firstSpace);
    trim(url);

    if (iequals(firstLine.substr(0, 4), "post"))
    {
        return Return(client.Post(url, ParseRequestContent(headers, lines)));
    }

    if (iequals(firstLine.substr(0, 4), "put"))
    {
        return Return(client.Put(url, ParseRequestContent(headers, lines)));
    }

    if (iequals(firstLine.substr(0, 4), "patch"))
    {
        return Return(client.Patch(url, ParseRequestContent(headers, lines)));
    }

    if (iequals(firstLine.substr(0, 4), "delete"))
    {
        return Return(client.Delete(url));
    }

    return Return(client.Get(url));
}

std::shared_ptr<HttpContent> FileRunnerService::ParseRequestContent(
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    std::ostringstream imploded;
    std::copy(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(imploded, "\n"));

    auto httpContent = std::make_shared<StringContent>(imploded.str());

    return httpContent;
}
