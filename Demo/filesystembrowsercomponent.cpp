#include "filesystembrowsercomponent.hpp"

#include "filesystemservice.hpp"
#include <glad/glad.h>

const int browserItemHeight = 30;

FileSystemBrowserComponent::FileSystemBrowserComponent(
    std::unique_ptr<Font> &font)
    : _font(font)
{}

bool FileSystemBrowserComponent::init(
    const glm::vec2 &origin)
{
    _origin = origin;

    tabItemMargin.Bottom = tabItemMargin.Top = 4;
    tabItemMargin.Left = tabItemMargin.Right = 4;

    tabItemPadding.Bottom = tabItemPadding.Top = 2;
    tabItemPadding.Left = tabItemPadding.Right = 10;

    _browserScrollBarFrom = std::make_shared<ScrollBarComponent>();
    _browserScrollBarFrom->init(glm::vec2(_origin.x, _origin.y));

    _browserScrollBarFrom->resize(_origin.x, _origin.y, _width, _height);

    _browserScrollBarFrom->onScrollY = [&](int diff) {
        auto a = (_totalBrowserLines / float(_height));
        auto amount = float(diff * a);
        ScrollY(amount);
    };
    _browserScrollBarFrom->getScrollInfo = [&](float &start, float &length) {
        start = _browserTopLine / float(_totalBrowserLines);
        length = (float(_height) / float(browserItemHeight + tabItemMargin.Top + tabItemMargin.Bottom)) / float(_totalBrowserLines);
    };

    return true;
}

void FileSystemBrowserComponent::render(
    const struct InputState &inputState)
{
    const float border = 4.0f;

    (void)inputState;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
    glVertex2f(_origin.x, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y - border);
    glVertex2f(_origin.x, _origin.y - border);
    glEnd();

    glBegin(GL_QUADS);
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
    glVertex2f(_origin.x, _origin.y - border);
    glVertex2f(_origin.x + _width, _origin.y - border);
    glVertex2f(_origin.x + _width, _origin.y + _height);
    glVertex2f(_origin.x, _origin.y + _height);
    glEnd();

    auto folders = FileSystem.GetFolders(_relativePathToOpenFolder);
    auto files = FileSystem.GetFiles(_relativePathToOpenFolder);

    float x = _origin.x;
    float y = _origin.y - (_browserTopLine * browserItemHeight);

    double plane[][4] = {
        {1, 0, 0, -_origin.x},
        {-1, 0, 0, _origin.x + _width},
        {0, 1, 0, -(_origin.y)},
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

    if (!_relativePathToOpenFolder.empty())
    {
        auto text = "< " + _relativePathToOpenFolder.string();

        scr::Rectangle border = GetBorderRectangleForFile(text, x, y);

        auto hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

        glBegin(GL_QUADS);
        glColor4f(0.4f, 0.4f, 0.4f, hover ? 1.0f : 0.0f);
        glVertex2f(border.left, border.top);
        glVertex2f(border.right, border.top);
        glVertex2f(border.right, border.bottom);
        glVertex2f(border.left, border.bottom);
        glEnd();

        float xbase = x + 2 + tabItemMargin.Left + tabItemPadding.Left;
        float ybase = y + tabItemMargin.Top + tabItemPadding.Top;
        DrawTextBase(_font, xbase, ybase + 20.0f, text.c_str(), text.size(), textFore);

        y = border.bottom + tabItemMargin.Bottom;
    }

    for (auto &folder : folders)
    {
        float xbase = x + 2 + tabItemMargin.Left + tabItemPadding.Left;
        float ybase = y + tabItemMargin.Top + tabItemPadding.Top;

        auto text = "+ " + std::filesystem::relative(folder, _relativePathToOpenFolder).string();

        scr::Rectangle border = GetBorderRectangleForFile(text, x, y);

        auto hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

        glBegin(GL_QUADS);
        glColor4f(0.4f, 0.4f, 0.4f, hover ? 1.0f : 0.0f);
        glVertex2f(border.left, border.top);
        glVertex2f(border.right, border.top);
        glVertex2f(border.right, border.bottom);
        glVertex2f(border.left, border.bottom);
        glEnd();

        DrawTextBase(_font, xbase, ybase + 20.0f, text.c_str(), text.size(), textFore);

        y = border.bottom;
    }

    for (auto &file : files)
    {
        float xbase = x + 2 + tabItemMargin.Left + tabItemPadding.Left;
        float ybase = y + tabItemMargin.Top + tabItemPadding.Top;

        auto text = "  " + std::filesystem::relative(file, _relativePathToOpenFolder).string();

        scr::Rectangle border = GetBorderRectangleForFile(text, x, y);

        auto hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

        glBegin(GL_QUADS);
        glColor4f(0.4f, 0.4f, 0.4f, hover ? 1.0f : 0.0f);
        glVertex2f(border.left, border.top);
        glVertex2f(border.right, border.top);
        glVertex2f(border.right, border.bottom);
        glVertex2f(border.left, border.bottom);
        glEnd();

        DrawTextBase(_font, xbase, ybase + 20.0f, text.c_str(), text.size(), textFore);

        y = border.bottom;
    }

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);

    _totalBrowserLines = 1 + (files.size() + folders.size());

    _browserScrollBarFrom->render(inputState);
}

void FileSystemBrowserComponent::resize(
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

    _browserScrollBarFrom->resize(_origin.x, _origin.y, _width, _height);
}

bool FileSystemBrowserComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool FileSystemBrowserComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool FileSystemBrowserComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

bool FileSystemBrowserComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(event.x, event.y)))
    {
        return false;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (_browserScrollBarFrom->handleMouseButtonInput(event, inputState))
        {
            return true;
        }

        auto folders = FileSystem.GetFolders(_relativePathToOpenFolder);
        auto files = FileSystem.GetFiles(_relativePathToOpenFolder);

        float x = _origin.x;
        float y = _origin.y - (_browserTopLine * browserItemHeight);

        if (!_relativePathToOpenFolder.empty())
        {
            auto text = "< " + _relativePathToOpenFolder.string();

            scr::Rectangle rect = GetBorderRectangleForFile(text, x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = _relativePathToOpenFolder.parent_path();

                return true;
            }

            y = rect.bottom;
        }

        for (auto &folder : folders)
        {
            auto text = folder.string();

            scr::Rectangle rect = GetBorderRectangleForFile(text, x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = folder;

                return true;
            }

            y = rect.bottom;
        }

        for (auto &file : files)
        {
            auto text = file.string();

            scr::Rectangle rect = GetBorderRectangleForFile(text, x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                auto fullPath = FileSystem.GetFullPath(text);

                if (onFileLoad)
                {
                    onFileLoad(FileSystem.GetFullPath(text));
                }

                return true;
            }

            y = rect.bottom;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (_browserScrollBarFrom->handleMouseButtonInput(event, inputState))
        {
            return true;
        }
    }

    return false;
}

bool FileSystemBrowserComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (_browserScrollBarFrom->handleMouseMotionInput(event, inputState))
    {
        return true;
    }

    return false;
}

bool FileSystemBrowserComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (_browserScrollBarFrom->handleMouseWheel(event, inputState))
    {
        return true;
    }

    ScrollY((event.y / glm::abs(event.y)) * 10);

    return false;
}

void FileSystemBrowserComponent::ScrollY(
    float amount)
{
    _browserTopLine -= amount;

    if (_browserTopLine < 0)
    {
        _browserTopLine = 0;
    }

    if (_browserTopLine >= _totalBrowserLines)
    {
        _browserTopLine = _totalBrowserLines;
    }
}

scr::Rectangle FileSystemBrowserComponent::GetBorderRectangleForFile(
    const std::string &text,
    float &x,
    float &y)
{
    scr::Rectangle rect;

    rect.top = y + tabItemMargin.Top;
    rect.left = _origin.x;
    rect.right = _origin.x + _width;
    rect.bottom = rect.top + browserItemHeight + tabItemPadding.Top + tabItemPadding.Bottom;

    return rect;
}
