#ifndef STRINGCONTENT_HPP
#define STRINGCONTENT_HPP

#include "bytearraycontent.hpp"

class StringContent : public ByteArrayContent
{
public:
    StringContent(
        const std::string &content);

    virtual ~StringContent();
};

#endif // STRINGCONTENT_HPP
