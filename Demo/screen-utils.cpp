#include "screen-utils.hpp"

#include <glad/glad.h>

namespace scr
{

    void RenderQuad(
        const glm::vec4 &color,
        const Rectangle &rc)
    {
        glBegin(GL_QUADS);
        glColor4f(color.r, color.g, color.b, color.a);
        glVertex2f(rc.left, rc.top);
        glVertex2f(rc.right, rc.top);
        glVertex2f(rc.right, rc.bottom);
        glVertex2f(rc.left, rc.bottom);
        glEnd();
    }

} // namespace scr
