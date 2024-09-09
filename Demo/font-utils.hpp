#ifndef FONT_UTILS_HPP
#define FONT_UTILS_HPP

#include <Platform.h>
#include <glm/glm.hpp>
#include <memory>

void DrawTextBase(
    std::unique_ptr<Font> &font_,
    float xbase,
    float ybase,
    const char *s,
    size_t len,
    glm::vec4 fore);

float WidthText(
    std::unique_ptr<Font> &font_,
    const char *s,
    size_t len);

extern glm::vec4 textFore;

#endif // FONT_UTILS_HPP
