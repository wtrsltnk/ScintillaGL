
#include "tabbededitorscomponent.hpp"

#include "font-utils.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <sstream>

const int tabBarHeight = 45;

glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

TabbedEditorsComponent::TabbedEditorsComponent(
    std::unique_ptr<Font> &font)
    : _font(font)
{
}

bool TabbedEditorsComponent::init(
    const glm::vec2 &origin)
{
    _origin = origin;

    tabMargin.Bottom = tabMargin.Top = 4;
    tabMargin.Left = tabMargin.Right = 4;

    tabPadding.Bottom = tabPadding.Top = 2;
    tabPadding.Left = tabPadding.Right = 10;

    fileSystemBrowser = std::make_shared<FileSystemBrowserComponent>(_font);
    fileSystemBrowser->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));

    fileSystemBrowser->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    fileSystemBrowser->onFileLoad = [&](const std::filesystem::path &fullPath) {
        if (switchedFrom != nullptr)
        {
            auto switchedFromEditor = dynamic_cast<TabbedEditorsComponent *>(switchedFrom.get());
            if (switchedFromEditor != nullptr)
            {
                switchedFromEditor->loadFile(fullPath.generic_string());
            }
            else
            {
                loadFile(fullPath.generic_string());
            }
        }
        else
        {
            loadFile(fullPath.generic_string());
        }
    };

    return true;
}

void TabbedEditorsComponent::loadFile(
    const std::filesystem::path &fileName)
{
    for (size_t i = 0; i < tabs.size(); i++)
    {
        if (tabs[i]->openFile == fileName)
        {
            _activeTab = i;
            return;
        }
    }

    std::stringstream buffer;

    std::ifstream t(fileName.c_str());
    buffer << t.rdbuf();

    auto editorLayer = std::make_shared<EditorComponent>();
    editorLayer->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));
    editorLayer->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    editorLayer->openFile = std::filesystem::path(fileName);
    editorLayer->title = editorLayer->openFile.filename().generic_string();
    editorLayer->loadContent(buffer.str());
    _activeTab = tabs.size();
    tabs.push_back(std::move(editorLayer));
}

void TabbedEditorsComponent::newTab(
    bool switchTo)
{
    auto editorLayer = std::make_shared<EditorComponent>();
    editorLayer->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));
    editorLayer->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    editorLayer->title = "empty.c";
    tabs.push_back(std::move(editorLayer));

    if (switchTo)
    {
        _activeTab = tabs.size() - 1;
    }
}

void TabbedEditorsComponent::closeTab(
    size_t index)
{
    if (tabs.empty())
    {
        return;
    }

    tabs.erase(std::next(tabs.begin(), index));

    if (_activeTab >= index)
    {
        _activeTab--;
    }
}

void TabbedEditorsComponent::RenderTab(
    const struct InputState &inputState,
    const std::string &text,
    float &x,
    float &y,
    bool isActiveTab)
{
    auto border = GetBorderRectangle(text, x, y);

    bool hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        hover = false;
    }

    // auto dragAmount = inputState.mouseX - _draggingStartX;

    // if (!isActiveTab || !_draggingTab)
    // {
    //     dragAmount = 0;
    // }

    auto alpha = isActiveTab ? 1.0f : (hover ? 0.6f : 0.4f);

    scr::FillQuad({0.2f, 0.2f, 0.2f, alpha}, border);

    DrawTextBase(
        _font,
        border.left + tabMargin.Left + tabPadding.Left,
        border.top + tabMargin.Top + tabPadding.Top + 20.0f,
        text,
        textFore);

    x = border.right;
}

void TabbedEditorsComponent::render(
    const struct InputState &inputState)
{
    (void)inputState;

    float x = _origin.x;
    float y = _origin.y;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!tabs.empty())
    {
        if (IComponent::componentWithKeyboardFocus.get() == this)
        {
            tabs[_activeTab]->tick();
        }

        for (const auto &tab : tabs)
        {
            auto title = tab->title;
            if (title.empty())
            {
                title = "unnamed";
            }

            bool isActiveTab = tabs[_activeTab] == tab;

            RenderTab(inputState, tab->title, x, y, isActiveTab);
        }

        if (!tabs.empty() && tabs.size() > _activeTab)
        {
            tabs[_activeTab]->render(inputState);
        }
    }
    else
    {
        fileSystemBrowser->render(inputState);
    }
}

void TabbedEditorsComponent::resize(
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

    fileSystemBrowser->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    for (const auto &tab : tabs)
    {
        tab->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);
    }
}

bool TabbedEditorsComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    switch (event.keysym.sym)
    {
        case SDLK_n:
            if (inputState.ctrl)
            {
                newTab(true);

                return true;
            }
        case SDLK_F4:
        {
            if (inputState.ctrl)
            {
                closeTab(_activeTab);
            }
            break;
        }
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleKeyDown(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleKeyDown(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleKeyUp(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleKeyUp(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleTextInput(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleTextInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(event.x, event.y)))
    {
        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseButtonInput(event, inputState))
        {
            return true;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        float x = _origin.x, y = _origin.y;

        for (size_t i = 0; i < tabs.size(); i++)
        {
            const auto &tab = tabs[i];
            auto border = GetBorderRectangle(tab->title, x, y);

            if (border.Contains(glm::vec2(event.x, event.y)))
            {
                if (event.button == SDL_BUTTON_LEFT)
                {
                    _activeTab = i;

                    _draggingTab = true;
                    _draggingStartX = event.x;

                    return true;
                }
                else if (event.button == SDL_BUTTON_MIDDLE)
                {
                    closeTab(i);
                    return true;
                }
            }

            x = border.right;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        _draggingTab = false;
    }

    if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseButtonInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (inputState.leftMouseDown && _draggingTab)
    {
        return true;
    }

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseMotionInput(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseMotionInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseWheel(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseWheel(event, inputState);
    }

    return false;
}

scr::Rectangle TabbedEditorsComponent::GetBorderRectangle(
    const std::string &text,
    float &x,
    float &y)
{
    float width = WidthText(_font, text);

    scr::Rectangle border;

    border.top = y + tabMargin.Top;
    border.bottom = y + tabMargin.Top + tabPadding.Top      // Left margin and padding
                    + tabBarHeight                          // This is the tab height
                    + tabPadding.Bottom + tabMargin.Bottom; // Right padding and margin

    border.left = x + tabMargin.Left;
    border.right = x + tabMargin.Left + tabPadding.Left  // Left margin and padding
                   + width                               // This is the text with
                   + tabPadding.Right + tabMargin.Right; // Right padding and margin

    return border;
}
