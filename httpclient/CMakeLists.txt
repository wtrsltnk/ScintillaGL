cmake_minimum_required(VERSION 3.12)

project(httpclient VERSION "0.1.0")

add_library(httpclient)

target_sources(httpclient
    PRIVATE
      "bytearraycontent.cpp"
      "bytearraycontent.hpp"
      "ca_cert.h"
      "httpclient.cpp"
      "httpclient.hpp"
      "httpcontent.cpp"
      "httpcontent.hpp"
      "httpmessagehandler.cpp"
      "httpmessagehandler.hpp"
      "httpmethod.hpp"
      "httprequestmessage.cpp"
      "httprequestmessage.hpp"
      "httpresponsemessage.cpp"
      "httpresponsemessage.hpp"
      "httpstatuscode.hpp"
      "jsoncontent.cpp"
      "jsoncontent.hpp"
      "stringcontent.cpp"
      "stringcontent.hpp"
)

target_compile_features(httpclient
    PRIVATE
        cxx_nullptr
        cxx_std_17
)

target_include_directories(httpclient
    PUBLIC
        "."
)

target_link_libraries(httpclient
    PRIVATE
        nlohmann_json::nlohmann_json
        mbedtls
        Wininet
)
