#include "filesystembrowsercomponent.hpp"

#include "filesystemservice.hpp"
#include "font-utils.hpp"
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

    _browserScrollBarFrom->resize(int(_origin.x), int(_origin.y), _width, _height);

    _browserScrollBarFrom->onScrollY = [&](int diff) {
        auto a = (_totalBrowserLines / float(_height));
        auto amount = float(diff * a);
        ScrollY(amount);
    };
    _browserScrollBarFrom->getScrollInfo = [&](float &start, float &length) {
        start = _browserTopLine / float(_totalBrowserLines);
        length = ListItemsInView() / float(_totalBrowserLines);
    };

    return true;
}

void FileSystemBrowserComponent::RenderListItem(
    const struct InputState &inputState,
    const std::string &text,
    float &x,
    float &y)
{
    scr::Rectangle border = GetBorderRectangleForFile(x, y);

    auto hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.4f, 0.4f, hover ? 1.0f : 0.0f);
    glVertex2f(border.left, border.top);
    glVertex2f(border.right, border.top);
    glVertex2f(border.right, border.bottom);
    glVertex2f(border.left, border.bottom);
    glEnd();

    DrawTextBase(
        _font,
        border.left + tabItemPadding.Left,
        border.top + tabItemPadding.Top + 20.0f,
        text,
        textFore);

    y = border.bottom + tabItemMargin.Bottom;
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
    float y = _origin.y - (_browserTopLine * (tabItemMargin.Top + browserItemHeight + tabItemMargin.Bottom));

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

    _totalBrowserLines = 0;

    if (!_relativePathToOpenFolder.empty())
    {
        _totalBrowserLines++;

        auto text = "< " + _relativePathToOpenFolder.string();

        RenderListItem(inputState, text, x, y);
    }

    for (auto &folder : folders)
    {
        _totalBrowserLines++;

        auto text = "+ " + std::filesystem::relative(folder, _relativePathToOpenFolder).string();

        RenderListItem(inputState, text, x, y);
    }

    for (auto &file : files)
    {
        _totalBrowserLines++;

        auto text = "  " + std::filesystem::relative(file, _relativePathToOpenFolder).string();

        RenderListItem(inputState, text, x, y);
    }

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);

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
        _origin.x = float(x);
    }
    else
    {
        x = int(_origin.x);
    }

    if (y >= 0)
    {
        _origin.y = float(y);
    }
    else
    {
        y = int(_origin.y);
    }

    _browserScrollBarFrom->resize(x, y, _width, _height);
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
        float y = _origin.y - (_browserTopLine * (tabItemMargin.Top + browserItemHeight + tabItemMargin.Bottom));

        if (!_relativePathToOpenFolder.empty())
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = _relativePathToOpenFolder.parent_path();

                return true;
            }

            y = rect.bottom + tabItemMargin.Bottom;
        }

        for (auto &folder : folders)
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = folder.string();

                return true;
            }

            y = rect.bottom + tabItemMargin.Bottom;
        }

        for (auto &file : files)
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                auto fullPath = FileSystem.GetFullPath(file.string());

                if (onFileLoad)
                {
                    onFileLoad(FileSystem.GetFullPath(file.string()));
                }

                return true;
            }

            y = rect.bottom + tabItemMargin.Bottom;
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

    auto maxLines = _totalBrowserLines;

    if (_browserTopLine >= maxLines)
    {
        _browserTopLine = maxLines;
    }
}

scr::Rectangle FileSystemBrowserComponent::GetBorderRectangleForFile(
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

int FileSystemBrowserComponent::ListItemsInView()
{
    return int(float(_height) / float(tabItemMargin.Top + browserItemHeight + tabItemMargin.Bottom));
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
