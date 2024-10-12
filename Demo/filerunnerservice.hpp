#ifndef FILERUNNERSERVICE_HPP
#define FILERUNNERSERVICE_HPP

#include <httpcontent.hpp>
#include <memory>
#include <string>

enum FileTypes
{
    Http,
    C,
    Sql,
    Sqlite,
};

class FileRunnerService
{
public:
    FileRunnerService();

    std::string Execute(
        const std::string &title,
        const std::string &content);

private:
    std::string ExecuteSql(
        const std::string &firstLine,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::string ExecuteSqlite(
        const std::string &connectionString,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::string ExecuteMssql(
        const std::string &connectionString,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::string ExecuteMysql(
        const std::string &connectionString,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::string ExecuteHttp(
        const std::string &firstLine,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::string ExecuteC(
        const std::string &firstLine,
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);

    std::shared_ptr<HttpContent> ParseRequestContent(
        const std::map<std::string, std::string> &headers,
        const std::vector<std::string> &lines);
};

#endif // FILERUNNERSERVICE_HPP
