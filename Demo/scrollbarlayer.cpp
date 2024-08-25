
#include "scrollbarlayer.hpp"

#include <glad/glad.h>

bool ScrollBarLayer::init(const glm::vec2 &origin)
{
    _origin = origin;

    return true;
}

void ScrollBarLayer::render(const struct InputState &inputState)
{
    (void)inputState;

    auto scrollBarHeight = _height - _origin.y;

    float start = 0.0f, length = _height;
    if (getScrollInfo)
    {
        getScrollInfo(start, length);
    }

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(0.7f, 0.7f, 0.7f, _hoverScroll || _scrolling ? 1.0f : 0.4f);

    glVertex2f(_width - scrollBarWidth, _origin.y + scrollBarHeight * start);
    glVertex2f(_width, _origin.y + scrollBarHeight * start);
    glVertex2f(_width, _origin.y + scrollBarHeight * (start + length));
    glVertex2f(_width - scrollBarWidth, _origin.y + scrollBarHeight * (start + length));
    glEnd();
}

void ScrollBarLayer::resize(int x, int y, int w, int h)
{
    _width = w;
    _height = h;
    _origin.x = x;
    _origin.y = y;
}

bool ScrollBarLayer::handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarLayer::handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarLayer::handleTextInput(SDL_TextInputEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarLayer::handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState)
{
    (void)inputState;

    if (event.state == SDL_RELEASED)
    {
        _scrolling = false;
    }

    if (event.x > (_width - scrollBarWidth))
    {
        if (event.state == SDL_PRESSED)
        {
            _scrolling = true;
            _startValue = event.y;
        }

        return true;
    }

    return false;
}

bool ScrollBarLayer::handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState)
{
    (void)inputState;

    _hoverScroll = event.x > (_width - (scrollBarWidth * 5));

    if (_scrolling)
    {
        onScrollY(_startValue - event.y);
        _startValue = event.y;

        return true;
    }

    return false;
}

bool ScrollBarLayer::handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}
