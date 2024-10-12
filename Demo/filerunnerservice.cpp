#include "filerunnerservice.hpp"

#include "stringhelpers.hpp"
#include <filesystem>
#include <httpclient.hpp>
#include <string>
#include <stringcontent.hpp>
#include <vector>

FileRunnerService::FileRunnerService() {}

std::string FileRunnerService::Execute(
    const std::string &title,
    const std::string &content)
{
    auto ext = std::filesystem::path(title).extension();

    auto lines = split_string(content, "\n");

    if (lines.empty())
    {
        return "// ERR: No content to execute";
    }

    auto firstLine = lines.front();
    lines.erase(lines.begin());
    trimComment(firstLine);

    size_t headersEndAt;
    auto headers = ParseHeaders(lines, headersEndAt);
    auto linesWithoutHeaders = std::vector<std::string>(lines.begin() + headersEndAt, lines.end());

    if (ext == ".http")
    {
        return ExecuteHttp(firstLine, headers, linesWithoutHeaders);
    }

    if (ext == ".sql")
    {
        return ExecuteSql(firstLine, headers, linesWithoutHeaders);
    }

    if (ext == ".c")
    {
        return ExecuteC(firstLine, headers, linesWithoutHeaders);
    }

    return "Unsupported file extension";
}

std::string FileRunnerService::ExecuteSql(
    const std::string &firstLine,
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    auto sqlType = firstLine;

    trimComment(sqlType);

    auto firstSpace = ltrim_copy(sqlType).find_first_of(' ');
    if (firstSpace == std::string::npos)
    {
        return "// ERR: Could not determine connectionstring type";
    }

    auto connectionString = sqlType.substr(firstSpace);
    trim(connectionString);

    if (iequals(sqlType.substr(0, 6), "sqlite"))
    {
        return ExecuteSqlite(connectionString, headers, lines);
    }

    if (iequals(sqlType.substr(0, 5), "mssql"))
    {
        return ExecuteMssql(connectionString, headers, lines);
    }

    if (iequals(sqlType.substr(0, 5), "mysql"))
    {
        return ExecuteMysql(connectionString, headers, lines);
    }

    return "empty";
}
