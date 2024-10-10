#include "filerunnerservice.hpp"

#include "stringhelpers.hpp"
#include <algorithm> // std::equal
#include <httpclient.hpp>
#include <iterator>
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <stringcontent.hpp>
#include <vector>

void cell(
    int type,
    const std::string &value,
    size_t columnWidth,
    std::stringstream &result)
{
    auto valueSize = std::min<size_t>(value.size(), columnWidth);
    auto prefix = std::string(columnWidth, ' ');

    if (type != SQLITE_TEXT)
    {
        result << prefix.substr(0, columnWidth - valueSize);
    }

    if (value.size() > columnWidth)
    {
        result << value.substr(0, valueSize - 2) << "..";
    }
    else
    {
        result << value.substr(0, valueSize);
    }

    if (type == SQLITE_TEXT)
    {
        result << prefix.substr(0, columnWidth - valueSize);
    }
}

void ErrorResult(
    sqlite3_stmt *ppStmt,
    int stepResult,
    std::stringstream &result)
{
    result << "Error: " << sqlite3_errstr(stepResult) << "\n\n";
}

void UpdateResult(
    sqlite3 *ppDb,
    std::stringstream &result)
{
    auto changes = sqlite3_changes(ppDb);

    result << "(" << changes;
    if (changes == 1)
    {
        result << " row";
    }
    else
    {
        result << " rows";
    }
    result << " affected)\n\n";
}

void RowResult(
    sqlite3_stmt *ppStmt,
    size_t columnWidth,
    std::stringstream &result)
{
    auto columnCount = sqlite3_column_count(ppStmt);

    auto prefix = std::string(columnWidth, ' ');
    std::vector<size_t> colWidths(columnCount);

    for (int col = 0; col < columnCount; col++)
    {
        if (col == 0)
            result << "| ";
        else
            result << " | ";

        auto name = std::string(sqlite3_column_name(ppStmt, col));

        trim(name);

        colWidths[col] = columnWidth;

        if (name.size() > columnWidth)
        {
            colWidths[col] = name.size();
        }

        cell(
            SQLITE_TEXT,
            name,
            colWidths[col],
            result);
    }

    result << " |\n";

    for (int col = 0; col < columnCount; col++)
    {
        if (col == 0)
            result << "+-";
        else
            result << "-+-";

        result << std::string(colWidths[col], '-');
    }

    result << "-+\n";

    while (true)
    {
        for (int col = 0; col < columnCount; col++)
        {
            if (col == 0)
                result << "| ";
            else
                result << " | ";

            cell(
                sqlite3_column_type(ppStmt, col),
                reinterpret_cast<const char *>(sqlite3_column_text(ppStmt, col)),
                colWidths[col],
                result);
        }

        result << " |\n";

        auto stepResult = sqlite3_step(ppStmt);

        if (stepResult != SQLITE_ROW)
        {
            if (stepResult != SQLITE_DONE)
            {
                ErrorResult(ppStmt, stepResult, result);
            }

            break;
        }
    }

    result << "\n";
}

template <class T>
T HeaderNumber(
    const std::map<std::string, std::string> &headers,
    const char *key,
    T fallBackValue)
{
    auto header = headers.find(key);
    if (header == headers.end())
    {
        return fallBackValue;
    }

    auto value = (T)std::atoi(header->second.c_str());

    return value;
}

std::string FileRunnerService::ExecuteSqlite(
    const std::string &connectionString,
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    sqlite3 *ppDb;
    auto openResult = sqlite3_open_v2(connectionString.c_str(), &ppDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (openResult != SQLITE_OK)
    {
        std::stringstream err;

        err << "Error: failed to open '" << connectionString << "':\n";
        err << sqlite3_errstr(openResult);

        return err.str();
    }

    std::ostringstream imploded;
    std::copy(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(imploded, "\n"));

    auto statement = imploded.str();

    std::stringstream result;

    sqlite3_stmt *ppStmt;
    const char *full = statement.c_str();
    const char *cursor = full;
    size_t size = statement.size();
    const char *tail = nullptr;

    auto columnWidth = HeaderNumber<size_t>(headers, "Column-Width", 20);

    if (columnWidth < 5)
    {
        columnWidth = 5;
    }

    while (size > 0)
    {
        auto prepareResult = sqlite3_prepare_v2(ppDb, cursor, int(size), &ppStmt, &tail);
        if (prepareResult != SQLITE_OK)
        {
            ErrorResult(ppStmt, prepareResult, result);

            sqlite3_close(ppDb);

            break;
        }

        auto stepResult = sqlite3_step(ppStmt);

        if (stepResult == SQLITE_DONE)
        {
            UpdateResult(ppDb, result);
        }
        else if (stepResult == SQLITE_ROW)
        {
            RowResult(ppStmt, columnWidth, result);
        }
        else
        {
            ErrorResult(ppStmt, stepResult, result);
        }

        size = statement.size() - (tail - full);

        cursor = tail;

        if (size < 10 && trim_copy(std::string(cursor)).empty())
        {
            break;
        }
    }

    sqlite3_close(ppDb);

    return result.str();
}
