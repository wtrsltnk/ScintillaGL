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
    virtual std::shared_ptr<std::istream> OnCreateContentReadStream();

private:
    std::shared_ptr<struct imemstream> _stream;
};

#endif // JSONCONTENT_HPP
