
#include "scrollbarcomponent.hpp"

#include "screen-utils.hpp"
#include <glad/glad.h>

bool ScrollBarComponent::init(
    const glm::vec2 &origin)
{
    _origin = origin;

    return true;
}

void ScrollBarComponent::render(
    const struct InputState &inputState)
{
    (void)inputState;

    _hoverScroll = (inputState.mouseX - _origin.x) > (_width - (scrollBarWidth * 5)) && (inputState.mouseX - _origin.x) < _width;

    float start = 0.0f, length = float(_height);
    if (getScrollInfo)
    {
        getScrollInfo(start, length);
    }

    if (length >= 1.0f)
    {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    scr::Rectangle rect({
        _origin.x + _width - scrollBarWidth,
        _origin.x + _width,
        _origin.y + _height * start,
        _origin.y + _height * (start + length),
    });

    scr::FillQuad({0.7f, 0.7f, 0.7f, _hoverScroll || _scrolling ? 1.0f : 0.4f}, rect);
    scr::DrawQuad({0.0f, 0.0f, 0.0f, _hoverScroll || _scrolling ? 1.0f : 0.4f}, rect);
}

void ScrollBarComponent::resize(
    int x,
    int y,
    int w,
    int h)
{
    _width = w;
    _height = h;
    _origin.x = float(x);
    _origin.y = float(y);
}

bool ScrollBarComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)inputState;

    _scrolling = false;

    if ((event.x - _origin.x) > (_width - scrollBarWidth))
    {
        if (event.state == SDL_PRESSED)
        {
            _scrolling = true;
            _startValue = int(event.y - _origin.y);
        }

        return true;
    }

    return false;
}

bool ScrollBarComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)inputState;

    if (_scrolling && !inputState.leftMouseDown)
    {
        _scrolling = false;
    }

    if (_scrolling)
    {
        onScrollY(_startValue - int(event.y - _origin.y));

        _startValue = int(event.y - _origin.y);

        return true;
    }

    return false;
}

bool ScrollBarComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool ScrollBarComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}
