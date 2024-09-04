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

        _panel1 = std::make_shared<SplitterComponent>(_font);
        _panel1->init(_origin);

        _panel2 = std::make_shared<SplitterComponent>(_font);
        _panel2->init(_origin);
    }
    else
    {
        _editor = std::make_shared<TabbedEditorsComponent>(_font);
        _editor->init(_origin);
    }

    resize(_origin.x, _origin.y, _width, _height);

    return true;
}

void SplitterComponent::render(
    const struct InputState &inputState)
{
    double plane[][4] = {
        {1, 0, 0, -_origin.x},
        {-1, 0, 0, _origin.x + _width},
        {0, 1, 0, -_origin.y},
        {0, -1, 0, _origin.y + _height},
    };
    glClipPlane(GL_CLIP_PLANE0, plane[0]);
    glClipPlane(GL_CLIP_PLANE1, plane[1]);
    glClipPlane(GL_CLIP_PLANE2, plane[2]);
    glClipPlane(GL_CLIP_PLANE3, plane[3]);
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);

    if (_editor != nullptr)
    {
        _editor->render(inputState);
    }
    else
    {
        _panel1->render(inputState);
        _panel2->render(inputState);

        scr::Rectangle rect = GetSplitBarRect();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto hover = GetSplitBarRect().Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

        glBegin(GL_QUADS);
        glColor4f(0.5f, 0.5f, 0.5f, hover ? 1.0f : 0.0f);
        glVertex2f(rect.left, rect.top);
        glVertex2f(rect.right, rect.top);
        glVertex2f(rect.right, rect.bottom);
        glVertex2f(rect.left, rect.bottom);
        glEnd();
    }
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
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
        const float splitterHalfSize = float(splitterSize) / 2.0f;
        if (_verticalSplitting)
        {
            _panel1->resize(
                _origin.x,
                _origin.y,
                (float(_width) * _splitAt) - splitterHalfSize,
                _height);
            _panel2->resize(
                _origin.x + (float(_width) * _splitAt) + splitterHalfSize,
                _origin.y,
                (float(_width) * (1.0f - _splitAt)) - splitterHalfSize,
                _height);
        }
        else
        {
            _panel1->resize(
                _origin.x,
                _origin.y,
                _width,
                (float(_height) * _splitAt) - splitterHalfSize);
            _panel2->resize(
                _origin.x,
                _origin.y + (float(_height) * _splitAt) + splitterHalfSize,
                _width,
                (float(_height) * (1.0f - _splitAt)) - splitterHalfSize);
        }
    }
}

scr::Rectangle SplitterComponent::GetSplitBarRect()
{
    scr::Rectangle rect;

    const float splitterHalfSize = float(splitterSize) / 2.0f;

    if (_verticalSplitting)
    {
        rect.left = _origin.x + (_width * _splitAt) - splitterHalfSize - 1;
        rect.right = rect.left + splitterSize;
        rect.top = _origin.y;
        rect.bottom = _origin.y + _height;
    }
    else
    {
        rect.left = _origin.x;
        rect.right = _origin.x + _width;
        rect.top = _origin.y + (_height * _splitAt) - splitterHalfSize;
        rect.bottom = rect.top + splitterSize;
    }

    return rect;
}

int SplitterComponent::width()
{
    return _width;
}

int SplitterComponent::height()
{
    return _height;
}

std::shared_ptr<TabbedEditorsComponent> &SplitterComponent::ActiveEditor()
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

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (GetSplitBarRect().Contains(glm::vec2(event.x, event.y)))
        {
            _isSplitMoving = true;

            _splitMovingStart = glm::vec2(event.x, event.y);

            return true;
        }
    }
    else
    {
        _isSplitMoving = false;
    }

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

    if (_isSplitMoving)
    {
        if (_verticalSplitting)
        {
            auto diff = float(event.x - _splitMovingStart.x) / float(_width);
            _splitAt += diff;

            if (_splitAt < 0.1f) _splitAt = 0.1f;
            if (_splitAt > 0.9f) _splitAt = 0.9f;

            _splitMovingStart.x = event.x;
        }
        else
        {
            auto diff = float(event.y - _splitMovingStart.y) / float(_height);
            _splitAt += diff;
            _splitMovingStart.y = event.y;
        }

        resize(_origin.x, _origin.y, _width, _height);

        return true;
    }

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
