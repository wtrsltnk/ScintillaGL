#include "splittercomponent.hpp"

#include <glad/glad.h>

const int splitterSize = 8;

SplitterComponent::SplitterComponent(
    std::unique_ptr<Font> &font)
    : _font(font)
{
}

bool SplitterComponent::init(
    const glm::vec2 &origin,
    float splitAt,
    bool vertical)
{
    _origin = origin;

    if (splitAt > 0.0f && splitAt < 1.0f)
    {
        _splitAt = splitAt;
        _verticalSplitting = vertical;

        _panel1 = std::make_unique<SplitterComponent>(_font);
        _panel1->init(_origin);

        _panel2 = std::make_unique<SplitterComponent>(_font);
        _panel2->init(_origin);
    }
    else
    {
        _editor = std::make_unique<TabbedEditorsComponent>(_font);
        _editor->init(_origin);
    }

    resize(_origin.x, _origin.y, _width, _height);

    return true;
}

void SplitterComponent::render(
    const struct InputState &inputState)
{
    if (_editor != nullptr)
    {
        _editor->render(inputState);
    }
    else
    {
        _panel1->render(inputState);
        _panel2->render(inputState);
        scr::Rectangle rect;

        if (_verticalSplitting)
        {
            rect.left = _origin.x + (float(_width - splitterSize) * _splitAt);
            rect.right = rect.left + splitterSize;
            rect.top = _origin.y;
            rect.bottom = _origin.y + _height;
        }
        else
        {
            rect.left = _origin.x;
            rect.right = _origin.x + _width;
            rect.top = _origin.y + (float(_height - splitterSize) * _splitAt);
            rect.bottom = rect.top + splitterSize;
        }

        glBegin(GL_QUADS);
        glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
        glVertex2f(rect.left, rect.top);
        glVertex2f(rect.right, rect.top);
        glVertex2f(rect.right, rect.bottom);
        glVertex2f(rect.left, rect.bottom);
        glEnd();
    }
}

void SplitterComponent::resize(
    int x,
    int y,
    int w,
    int h)
{
    _width = w;
    _height = h;

    if (x >= 0)
    {
        _origin.x = (float)x;
    }
    else
    {
        x = _origin.x;
    }

    if (y >= 0)
    {
        _origin.y = (float)y;
    }
    else
    {
        y = _origin.y;
    }

    if (_editor != nullptr)
    {
        _editor->resize(_origin.x, _origin.y, _width, _height);
    }
    else
    {
        if (_verticalSplitting)
        {
            _panel1->resize(
                _origin.x,
                _origin.y,
                float(_width - splitterSize) * _splitAt,
                _height);
            _panel2->resize(
                _origin.x + (float(_width + splitterSize) * _splitAt),
                _origin.y,
                float(_width - splitterSize) * (1.0f - _splitAt),
                _height);
        }
        else
        {
            _panel1->resize(
                _origin.x,
                _origin.y,
                _width,
                float(_height - splitterSize) * _splitAt);
            _panel2->resize(
                _origin.x,
                _origin.y + (float(_height + splitterSize) * _splitAt),
                _width,
                float(_height - splitterSize) * (1.0f - _splitAt));
        }
    }
}

int SplitterComponent::width()
{
    return _width;
}

int SplitterComponent::height()
{
    return _height;
}

std::unique_ptr<TabbedEditorsComponent> &SplitterComponent::ActiveEditor()
{
    if (_editor != nullptr)
    {
        return _editor;
    }

    return _panel1->ActiveEditor();
}

bool SplitterComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleKeyDown(event, inputState);
    }

    return false;
}

bool SplitterComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleKeyUp(event, inputState);
    }
    else
    {
        return _panel1->handleKeyUp(event, inputState) || _panel2->handleKeyUp(event, inputState);
    }

    return false;
}

bool SplitterComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleTextInput(event, inputState);
    }
    else
    {
        return _panel1->handleTextInput(event, inputState) || _panel2->handleTextInput(event, inputState);
    }

    return false;
}

bool SplitterComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleMouseButtonInput(event, inputState);
    }
    else
    {
        return _panel1->handleMouseButtonInput(event, inputState) || _panel2->handleMouseButtonInput(event, inputState);
    }

    return false;
}

bool SplitterComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleMouseMotionInput(event, inputState);
    }
    else
    {
        return _panel1->handleMouseMotionInput(event, inputState) || _panel2->handleMouseMotionInput(event, inputState);
    }

    return false;
}

bool SplitterComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        return _editor->handleMouseWheel(event, inputState);
    }
    else
    {
        return _panel1->handleMouseWheel(event, inputState) || _panel2->handleMouseWheel(event, inputState);
    }

    return false;
}
