#include "filerunnerservice.hpp"

#include <filesystem>
#include <httpclient.hpp>
#include <iostream>
#include <stringcontent.hpp>

FileRunnerService::FileRunnerService() {}

std::string FileRunnerService::Execute(
    const std::string &title,
    const std::string &content)
{
    auto ext = std::filesystem::path(title).extension();
    std::cout << ext << std::endl;

    if (ext == ".http")
    {
        HttpClient client;

        auto request = std::make_shared<StringContent>(R"({
              "model": "mistral",
              "messages": [
                { "role": "user", "content": "why is the sky blue?" }
              ]
            })");

        auto response = client.Post("http://localhost:11434/api/generate", request);

        return response->Content->ReadAsString();
    }

    return "Unsupported file extension";
}
