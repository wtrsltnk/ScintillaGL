#include "splittercomponent.hpp"

#include <glad/glad.h>

const int splitterSize = 8;

SplitterComponent::SplitterComponent(
    const std::unique_ptr<FileRunnerService> &fileRunnerService,
    std::unique_ptr<Font> &font,
    std::unique_ptr<Font> &iconFont)
    : _fileRunnerService(fileRunnerService), _font(font), _iconFont(iconFont)
{
}

bool SplitterComponent::init(
    const glm::vec2 &origin,
    float splitAt,
    bool vertical,
    std::shared_ptr<TabbedEditorsComponent> editor)
{
    _origin = origin;

    if (splitAt > 0.0f && splitAt < 1.0f)
    {
        _splitAt = splitAt;
        _verticalSplitting = vertical;

        _panel1 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
        _panel1->init(_origin);

        _panel2 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
        _panel2->init(_origin);
    }
    else
    {
        _editor = editor;

        if (_editor == nullptr)
        {
            _editor = std::make_shared<TabbedEditorsComponent>(_fileRunnerService, _font, _iconFont);
        }

        _editor->init(_origin);

        IComponent::componentWithKeyboardFocus = _editor;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (_editor != nullptr)
    {
        _editor->render(inputState);

        auto rect1 = GetAddSplitButtonRect1();

        auto alpha1 = rect1.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)) ? 1.0f : 0.0f;

        scr::FillQuad({0.5f, 0.5f, 0.5f, alpha1}, rect1);

        if (!_editor->tabs.empty())
        {
            auto rect2 = GetAddSplitButtonRect2();

            auto alpha2 = rect2.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)) ? 1.0f : 0.0f;

            scr::FillQuad({0.5f, 0.5f, 0.5f, alpha2}, rect2);
        }
    }
    else
    {
        _panel1->render(inputState);
        _panel2->render(inputState);

        auto rect1 = GetSplitBarRect();

        auto alpha1 = rect1.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)) ? 1.0f : 0.0f;

        scr::FillQuad({0.5f, 0.5f, 0.5f, alpha1}, rect1);
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
        auto rect = GetSplitBarRect();
        const float splitterHalfSize = float(splitterSize) / 2.0f;
        if (_verticalSplitting)
        {
            _panel1->resize(
                _origin.x,
                _origin.y,
                rect.left - _origin.x,
                _height);
            _panel2->resize(
                rect.right,
                _origin.y,
                _width - (rect.right - _origin.x),
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

scr::Rectangle SplitterComponent::GetAddSplitButtonRect1()
{
    const float buttonSize = 20.0f;

    scr::Rectangle rect;

    rect.left = _origin.x + _width - buttonSize;
    rect.right = _origin.x + _width;
    rect.top = _origin.y + _height - buttonSize;
    rect.bottom = _origin.y + _height;

    return rect;
}

scr::Rectangle SplitterComponent::GetAddSplitButtonRect2()
{
    const float buttonSize = 20.0f;

    scr::Rectangle rect;

    rect.left = _origin.x;
    rect.right = _origin.x + buttonSize;
    rect.top = _origin.y + _height - buttonSize;
    rect.bottom = _origin.y + _height;

    return rect;
}

std::shared_ptr<TabbedEditorsComponent> &SplitterComponent::ActiveEditor()
{
    if (_editor != nullptr)
    {
        return _editor;
    }

    return _panel2->ActiveEditor();
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
        if (_editor != nullptr)
        {
            if (_editor->isHit(glm::vec2(event.x, event.y)))
            {
                if (IComponent::componentWithKeyboardFocus != nullptr && IComponent::componentWithKeyboardFocus != _editor)
                {
                    _editor->switchedFrom = IComponent::componentWithKeyboardFocus;
                }
                IComponent::componentWithKeyboardFocus = _editor;
            }
        }
        else
        {
            if (_panel1->isHit(glm::vec2(event.x, event.y)))
            {
                auto editor = _panel1->ActiveEditor();

                if (IComponent::componentWithKeyboardFocus != nullptr && IComponent::componentWithKeyboardFocus != editor)
                {
                    editor->switchedFrom = IComponent::componentWithKeyboardFocus;
                }

                IComponent::componentWithKeyboardFocus = editor;
            }
            else if (_panel2->isHit(glm::vec2(event.x, event.y)))
            {
                auto editor = _panel2->ActiveEditor();

                if (IComponent::componentWithKeyboardFocus != nullptr && IComponent::componentWithKeyboardFocus != editor)
                {
                    editor->switchedFrom = IComponent::componentWithKeyboardFocus;
                }

                IComponent::componentWithKeyboardFocus = editor;
            }
        }

        if (_editor == nullptr && GetSplitBarRect().Contains(glm::vec2(event.x, event.y)))
        {
            _isSplitMoving = true;

            _splitMovingStart = glm::vec2(event.x, event.y);

            return true;
        }

        if (_editor != nullptr && GetAddSplitButtonRect1().Contains(glm::vec2(event.x, event.y)))
        {
            _isAddingSplit = 1;

            _addingSplitStart = glm::vec2(event.x, event.y);

            return true;
        }

        if (_editor != nullptr && !_editor->tabs.empty() && GetAddSplitButtonRect2().Contains(glm::vec2(event.x, event.y)))
        {
            _isAddingSplit = 2;

            _addingSplitStart = glm::vec2(event.x, event.y);

            return true;
        }
    }
    else
    {
        _isSplitMoving = false;
        _isAddingSplit = 0;
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

void SplitterComponent::CollapsePanel1()
{
    if (_panel1->_editor != nullptr)
    {
        _editor = _panel2->_editor;
        for (auto &tab : _panel1->_editor->tabs)
        {
            if (tab->isUnTouched())
            {
                continue;
            }

            _panel2->_editor->tabs.insert(_panel2->_editor->tabs.begin(), tab);
        }

        _panel1 = nullptr;
        _panel2 = nullptr;
    }
    else
    {
        _verticalSplitting = _panel2->_verticalSplitting;
        _splitAt = _panel2->_splitAt;
        auto p1 = _panel2->_panel1;
        auto p2 = _panel2->_panel2;
        _panel1 = p1;
        _panel2 = p2;
    }
}

void SplitterComponent::CollapsePanel2()
{
    if (_panel1->_editor != nullptr)
    {
        _editor = _panel1->_editor;
        for (auto &tab : _panel2->_editor->tabs)
        {
            if (tab->isUnTouched())
            {
                continue;
            }

            _panel1->_editor->tabs.insert(_panel2->_editor->tabs.begin(), tab);
        }

        _panel1 = nullptr;
        _panel2 = nullptr;
    }
    else
    {
        _verticalSplitting = _panel1->_verticalSplitting;
        _splitAt = _panel1->_splitAt;
        auto p1 = _panel1->_panel1;
        auto p2 = _panel1->_panel2;
        _panel1 = p1;
        _panel2 = p2;
    }
}

void SplitterComponent::AddVerticalSplit(
    const glm::vec2 &mouse)
{
    _splitAt = glm::abs(_origin.x - mouse.x) / float(_width);
    _verticalSplitting = true;

    _panel1 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
    _panel1->init(_origin, 0.0f, false, _isAddingSplit == 1 ? _editor : nullptr);
    if (_isAddingSplit != 1)
    {
        _panel1->_editor->newTab("empty.c");
    }

    _panel2 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
    _panel2->init(_origin, 0.0f, false, _isAddingSplit == 2 ? _editor : nullptr);
    if (_isAddingSplit != 2)
    {
        _panel2->_editor->newTab("empty.c");
    }

    _editor = nullptr;

    _isSplitMoving = true;
    _splitMovingStart = mouse;

    resize(_origin.x, _origin.y, _width, _height);

    _isAddingSplit = 0;
}

void SplitterComponent::AddHorizontalSplit(
    const glm::vec2 &mouse)
{
    _splitAt = glm::abs(_origin.y - mouse.y) / float(_height);
    _verticalSplitting = false;

    _panel1 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
    _panel1->init(_origin, 0.0f, false, _isAddingSplit == 1 ? _editor : nullptr);

    _panel2 = std::make_shared<SplitterComponent>(_fileRunnerService, _font, _iconFont);
    _panel2->init(_origin, 0.0f, false, _isAddingSplit == 2 ? _editor : nullptr);

    _editor = nullptr;

    _isSplitMoving = true;
    _splitMovingStart = mouse;

    resize(_origin.x, _origin.y, _width, _height);

    _isAddingSplit = 0;
}

bool SplitterComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_editor != nullptr)
    {
        if (_isAddingSplit)
        {
            if (glm::abs((event.x - _addingSplitStart.x)) > 10.0f)
            {
                AddVerticalSplit(glm::vec2(event.x, event.y));

                return true;
            }

            if (glm::abs((event.y - _addingSplitStart.y)) > 10.0f)
            {
                AddHorizontalSplit(glm::vec2(event.x, event.y));

                return true;
            }

            return false;
        }

        return _editor->handleMouseMotionInput(event, inputState);
    }
    else
    {
        if (_isSplitMoving)
        {
            if (_verticalSplitting)
            {
                auto diff = float(event.x - _splitMovingStart.x) / float(_width);
                _splitAt += diff;

                _splitMovingStart.x = event.x;
            }
            else
            {
                auto diff = float(event.y - _splitMovingStart.y) / float(_height);
                _splitAt += diff;
                _splitMovingStart.y = event.y;
            }

            if (_splitAt <= 0.0f)
            {
                CollapsePanel1();
                _isSplitMoving = false;
            }
            else if (_splitAt >= 1.0f)
            {
                CollapsePanel2();
                _isSplitMoving = false;
            }

            resize(_origin.x, _origin.y, _width, _height);

            return true;
        }

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
