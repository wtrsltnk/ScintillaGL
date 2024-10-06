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
    std::string ExecuteHttp(
        const std::string &content);

    std::shared_ptr<HttpContent> ParseRequestContent(
        const std::vector<std::string> &lines);
};

#endif // FILERUNNERSERVICE_HPP
