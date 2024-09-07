#include "filesystemservice.hpp"

FileSystemService FileSystem;

FileSystemService::FileSystemService()
{
    _rootPath = std::filesystem::current_path();
}

void FileSystemService::SetRoot(
    const fspath &root)
{
    _rootPath = root;
}

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

std::vector<fspath> GetRealFolders(
    const fspath &root,
    const fspath &relativePath)
{
    auto result = std::vector<fspath>();

    std::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (std::filesystem::directory_iterator itr(root / relativePath);
         itr != end_itr;
         ++itr)
    {
        if (!is_directory(itr->status()))
        {
            continue;
        }

        result.push_back(std::filesystem::relative(*itr, root));
    }

    return result;
}

std::vector<fspath> GetRealFiles(
    const fspath &root,
    const fspath &relativePath)
{
    auto result = std::vector<fspath>();

    std::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (std::filesystem::directory_iterator itr(root / relativePath);
         itr != end_itr;
         ++itr)
    {
        if (is_directory(itr->status()))
        {
            continue;
        }

        result.push_back(std::filesystem::relative(*itr, root));
    }

    return result;
}

std::vector<fspath> FileSystemService::GetFolders(
    const std::filesystem::path &path,
    bool updateCache)
{
    auto found = _foldersCache.find(path);

    if (found != _foldersCache.end() && !updateCache)
    {
        return found->second;
    }

    if (found != _foldersCache.end())
    {
        _foldersCache.erase(found);
    }

    _foldersCache.insert(std::make_pair(path, GetRealFolders(_rootPath, path)));

    return _foldersCache[path];
}

std::vector<fspath> FileSystemService::GetFiles(
    const fspath &path,
    bool updateCache)
{
    auto found = _filesCache.find(path);

    if (found != _filesCache.end() && !updateCache)
    {
        return found->second;
    }

    if (found != _filesCache.end())
    {
        _filesCache.erase(found);
    }

    _filesCache.insert(std::make_pair(path, GetRealFiles(_rootPath, path)));

    return _filesCache[path];
}

fspath FileSystemService::GetFullPath(
    const fspath &relativePath)
{
    return _rootPath / relativePath;
}
