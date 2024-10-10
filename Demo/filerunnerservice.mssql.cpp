#include "filerunnerservice.hpp"

#include <algorithm> // std::equal
#include <httpclient.hpp>
#include <iterator>
#include <sstream>
#include <string>
#include <stringcontent.hpp>
#include <vector>

std::string FileRunnerService::ExecuteMssql(
    const std::string &connectionString,
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    std::ostringstream imploded;
    std::copy(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(imploded, "\n"));

    return imploded.str();
}
