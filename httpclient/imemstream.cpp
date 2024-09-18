#include "imemstream.h"

membuf::membuf(char const *base, size_t size)
{
    char *p(const_cast<char *>(base));
    this->setg(p, p, p + size);
}

imemstream::imemstream(char const *base, size_t size)
    : membuf(base, size), std::istream(static_cast<std::streambuf *>(this))
{}
