#ifndef FONT_UTILS_HPP
#define FONT_UTILS_HPP

#include <Platform.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

void DrawTextBase(
    std::unique_ptr<Font> &font_,
    float xbase,
    float ybase,
    const std::string &text,
    glm::vec4 fore);

float WidthText(
    std::unique_ptr<Font> &font_,
    const std::string &text);

extern glm::vec4 textFore;

#endif // FONT_UTILS_HPP
