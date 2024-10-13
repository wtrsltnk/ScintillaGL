#include "filerunnerservice.hpp"

#include <assert.h>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <libtcc.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

void handle_error(void *opaque, const char *msg)
{
    auto thiz = reinterpret_cast<std::stringstream *>(opaque);

    (*thiz) << msg << "\n";
}

typedef const char *(*initFunc)(int *);
typedef int (*filterFunc)(const char *);

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

void demo_perms(
    std::stringstream &result,
    std::filesystem::perms p)
{
    using std::filesystem::perms;
    auto show = [&](char op, perms perm) {
        result << (perms::none == (perm & p) ? '-' : op);
    };
    show('r', perms::owner_read);
    show('w', perms::owner_write);
    show('x', perms::owner_exec);
    show('r', perms::group_read);
    show('w', perms::group_write);
    show('x', perms::group_exec);
    show('r', perms::others_read);
    show('w', perms::others_write);
    show('x', perms::others_exec);
}

std::string FileRunnerService::ExecuteC(
    const std::string &firstLine,
    const std::map<std::string, std::string> &headers,
    const std::vector<std::string> &lines)
{
    TCCState *s;
    std::stringstream result;

    s = tcc_new();
    if (!s)
    {
        fprintf(stderr, "Could not create tcc state\n");
        exit(1);
    }

    tcc_set_error_func(s, &result, handle_error);

    tcc_add_library_path(s, "../lib");
    tcc_add_include_path(s, "../include");

    /* MUST BE CALLED before any compilation */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    std::ostringstream imploded;
    std::copy(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(imploded, "\n"));

    auto code = imploded.str();

    if (tcc_compile_string(s, code.c_str()) == -1)
    {
        return result.str();
    }

    /* relocate the code */
    if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
    {
        return result.str();
    }

    initFunc init = (initFunc)tcc_get_symbol(s, "init");
    if (!init)
    {
        result << "missing 'const char* init()\n{\n\treturn \"c:\\wtr\\temp\";}\n'";

        return result.str();
    }

    filterFunc filter = (filterFunc)tcc_get_symbol(s, "filter");
    if (!filter)
    {
        result << "missing 'int filter(const char*)\n{\n\treturn 0;\n}\n'";

        return result.str();
    }

    /* run the code */
    int flags = 0;
    auto initResult = init(&flags);

    result << "Searching in \"" << initResult << "\":\n\n";
    try
    {
        auto itr = std::filesystem::recursive_directory_iterator(initResult, std::filesystem::directory_options::skip_permission_denied);

#define SHOW_FILE_SIZE 1
#define SHOW_FILE_TIME 2
#define SHOW_FILE_PERMISSIONS 4

        const auto start = std::chrono::steady_clock::now();

        for (const auto &entry : itr)
        {
            if (filter(entry.path().string().c_str()) == 1)
            {
                if (flags & SHOW_FILE_SIZE)
                {
                    result << std::setw(10) << std::filesystem::file_size(entry.path()) << std::setw(0) << " | ";
                }

                if (flags & SHOW_FILE_TIME)
                {
                    auto time = std::filesystem::last_write_time(entry.path());
                    std::time_t tt = to_time_t(time);
                    std::tm *gmt = std::gmtime(&tt);

                    result << std::put_time(gmt, "%Y-%m-%d %H:%M") << " | ";
                }

                if (flags & SHOW_FILE_PERMISSIONS)
                {
                    demo_perms(result, std::filesystem::status(entry.path()).permissions());
                    result << " | ";
                }

                result << entry.path().string();

                result << "\n";
            }
        }

        const auto end = std::chrono::steady_clock::now();

        const std::chrono::duration<double> diff = end - start;

        result << "\n---------------------------------------------------------\nSearch returned in " << diff.count()
               << " seconds\n";
    }
    catch (const std::filesystem::filesystem_error &ex)
    {
        result << "Exception: " << ex.what() << " " << ex.code() << '\n';
    }

    /* delete the state */
    tcc_delete(s);

    return result.str();
}
