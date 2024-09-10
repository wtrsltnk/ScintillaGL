#include "filesystembrowsercomponent.hpp"

#include "filesystemservice.hpp"
#include "font-utils.hpp"
#include <glad/glad.h>

const int browserItemHeight = 24;

FileSystemBrowserComponent::FileSystemBrowserComponent(
    std::unique_ptr<Font> &font)
    : _font(font)
{}

bool FileSystemBrowserComponent::init(
    const glm::vec2 &origin)
{
    _origin = origin;

    margin.Bottom = margin.Top = 2;
    margin.Left = margin.Right = 4;

    padding.Bottom = padding.Top = 2;
    padding.Left = padding.Right = 10;

    _scrollBar = std::make_shared<ScrollBarComponent>();
    _scrollBar->init(glm::vec2(_origin.x, _origin.y));

    _scrollBar->resize(int(_origin.x), int(_origin.y), _width, _height);

    _scrollBar->onScrollY = [&](int diff) {
        auto a = float(_totalBrowserLines) / float(_height);
        auto amount = float(diff * a);
        ScrollY(amount);
    };

    _scrollBar->getScrollInfo = [&](float &start, float &length) {
        start = float(_browserTopLine) / float(_totalBrowserLines);
        length = float(ListItemsInView()) / float(_totalBrowserLines);
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

    scr::RenderQuad({0.4f, 0.4f, 0.4f, hover ? 1.0f : 0.0f}, border);

    DrawTextBase(
        _font,
        border.left + padding.Left,
        border.top + padding.Top + 20.0f,
        text,
        textFore);

    y = border.bottom;
}

void FileSystemBrowserComponent::render(
    const struct InputState &inputState)
{
    (void)inputState;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto folders = FileSystem.GetFolders(_relativePathToOpenFolder);
    auto files = FileSystem.GetFiles(_relativePathToOpenFolder);

    float x = _origin.x;
    float y = _origin.y - (int(_browserTopLine) * ItemHeightWithMarginAndPadding());

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

    _scrollBar->render(inputState);
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

    _scrollBar->resize(x, y, _width, _height);
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
        if (_scrollBar->handleMouseButtonInput(event, inputState))
        {
            return true;
        }

        auto folders = FileSystem.GetFolders(_relativePathToOpenFolder);
        auto files = FileSystem.GetFiles(_relativePathToOpenFolder);

        float x = _origin.x;
        float y = _origin.y - (int(_browserTopLine) * ItemHeightWithMarginAndPadding());

        if (!_relativePathToOpenFolder.empty())
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = _relativePathToOpenFolder.parent_path();

                return true;
            }

            y = rect.bottom;
        }

        for (auto &folder : folders)
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (event.button == SDL_BUTTON_LEFT && rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                _relativePathToOpenFolder = folder.string();

                return true;
            }

            y = rect.bottom;
        }

        for (auto &file : files)
        {
            scr::Rectangle rect = GetBorderRectangleForFile(x, y);

            if (event.button == SDL_BUTTON_LEFT && rect.Contains(glm::vec2(inputState.mouseX, inputState.mouseY)))
            {
                auto fullPath = FileSystem.GetFullPath(file.string());

                if (onFileLoad)
                {
                    onFileLoad(FileSystem.GetFullPath(file.string()));
                }

                return true;
            }

            y = rect.bottom;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (_scrollBar->handleMouseButtonInput(event, inputState))
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

    if (_scrollBar->handleMouseMotionInput(event, inputState))
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

    if (_scrollBar->handleMouseWheel(event, inputState))
    {
        return true;
    }

    ScrollY((event.y / glm::abs(event.y)) * 10.0f);

    return false;
}

void FileSystemBrowserComponent::ScrollY(
    float amount)
{
    if (_totalBrowserLines <= ListItemsInView())
    {
        _browserTopLine = 0;
        return;
    }

    _browserTopLine -= amount;

    if (_browserTopLine < 0)
    {
        _browserTopLine = 0;
    }

    if (_browserTopLine >= _totalBrowserLines - ListItemsInView())
    {
        _browserTopLine = _totalBrowserLines - ListItemsInView();
    }
}

scr::Rectangle FileSystemBrowserComponent::GetBorderRectangleForFile(
    float &x,
    float &y)
{
    scr::Rectangle rect;

    rect.top = y + margin.Top;
    rect.left = _origin.x;
    rect.right = _origin.x + _width;
    rect.bottom = y + ItemHeightWithMarginAndPadding();

    return rect;
}

float FileSystemBrowserComponent::ItemHeightWithMarginAndPadding()
{
    return float(margin.Top + padding.Top + browserItemHeight + padding.Bottom + margin.Bottom);
}

int FileSystemBrowserComponent::ListItemsInView()
{
    return int(glm::floor(float(_height) / ItemHeightWithMarginAndPadding()));
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
