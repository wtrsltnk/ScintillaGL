#ifndef FILESYSTEMSERVICE_HPP
#define FILESYSTEMSERVICE_HPP

#include <filesystem>
#include <map>
#include <vector>

typedef std::filesystem::path fspath;

class FileSystemService
{
public:
    FileSystemService();

    void SetRoot(
        const fspath &root);

    const fspath &GetRoot() const { return _rootPath; }

    std::vector<fspath> GetFolders(
        const fspath &path,
        bool updateCache = false);

    std::vector<fspath> GetFiles(
        const fspath &path,
        bool updateCache = false);

    fspath GetFullPath(
        const fspath &relativePath);

private:
    fspath _rootPath;
    std::map<fspath, std::vector<fspath>> _filesCache;
    std::map<fspath, std::vector<fspath>> _foldersCache;
};

extern FileSystemService FileSystem;

#endif // FILESYSTEMSERVICE_HPP
