#ifndef IMEMSTREAM_H
#define IMEMSTREAM_H

#include <istream>
#include <streambuf>

struct membuf : std::streambuf
{
    membuf(char const *base, size_t size);
};

struct imemstream : virtual membuf, std::istream
{
    imemstream(char const *base, size_t size);
};

#endif // IMEMSTREAM_H
