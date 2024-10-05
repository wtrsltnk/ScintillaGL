#ifndef FILERUNNERSERVICE_HPP
#define FILERUNNERSERVICE_HPP

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
};

#endif // FILERUNNERSERVICE_HPP
