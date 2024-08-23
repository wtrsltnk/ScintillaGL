#ifndef STBTT_FONT_HPP
#define STBTT_FONT_HPP

#include "stb_truetype.h"
#include <glad/glad.h>

struct stbtt_Font
{
    stbtt_fontinfo fontinfo;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint ftex;
    float scale;
};

#endif // STBTT_FONT_HPP
