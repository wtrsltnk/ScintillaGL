#ifndef SCREEN_UTILS_HPP
#define SCREEN_UTILS_HPP

#include <glm/glm.hpp>

namespace scr
{

    struct Padding
    {
        int Left;
        int Top;
        int Right;
        int Bottom;
    };

    struct Margin
    {
        int Left;
        int Top;
        int Right;
        int Bottom;
    };

    struct Rectangle
    {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;

        bool operator==(struct Rectangle &rc)
        {
            return (rc.left == left) && (rc.right == right) &&
                   (rc.top == top) && (rc.bottom == bottom);
        }

        const struct Rectangle &operator+=(struct Rectangle &rc)
        {
            if (rc.left < left) left = rc.left;
            if (rc.top < top) top = rc.top;

            if (rc.right > right) right = rc.right;
            if (rc.bottom > bottom) bottom = rc.bottom;

            return *this;
        }

        bool Contains(glm::vec2 pt)
        {
            return (pt.x >= left) && (pt.x <= right) &&
                   (pt.y >= top) && (pt.y <= bottom);
        }

        bool Contains(struct Rectangle rc)
        {
            return (rc.left >= left) && (rc.right <= right) &&
                   (rc.top >= top) && (rc.bottom <= bottom);
        }

        bool Intersects(struct Rectangle other)
        {
            return (right > other.left) && (left < other.right) &&
                   (bottom > other.top) && (top < other.bottom);
        }
    };

    void FillQuad(
        const glm::vec4 &color,
        const Rectangle &rc);

    void DrawQuad(
        const glm::vec4 &color,
        const Rectangle &rc);

    enum class Direction
    {
        Horizontal,
        Vertical,
    };

}; // namespace scr

#endif // SCREEN_UTILS_HPP
