#ifndef STRINGHELPERS_HPP
#define STRINGHELPERS_HPP

#include <map>
#include <string>
#include <vector>

std::map<std::string, std::string> ParseHeaders(
    const std::vector<std::string> &lines,
    size_t &headersEndAt);

bool ichar_equals(char a, char b);

bool iequals(const std::string &a, const std::string &b);

// trim from start (in place)
void ltrim(std::string &s);

// trim comment from start (in place)
void trimComment(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// trim from start (copying)
std::string ltrim_copy(std::string s);

// trim from end (copying)
std::string rtrim_copy(std::string s);

// trim from both ends (copying)
std::string trim_copy(std::string s);

std::vector<std::string> split_string(
    const std::string &str,
    const std::string &delimiter);

std::string escaped(
    const std::string &input);

#endif // STRINGHELPERS_HPP
