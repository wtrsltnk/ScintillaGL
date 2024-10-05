#ifndef JSONCONTENT_HPP
#define JSONCONTENT_HPP

#include "httpcontent.hpp"
#include <nlohmann/json.hpp>

class JsonContent : public HttpContent
{
public:
    JsonContent(
        const nlohmann::json &data);

    virtual ~JsonContent();

protected:
    const std::vector<std::byte> &Stream();

private:
    std::vector<std::byte> _stream;
};

#endif // JSONCONTENT_HPP
