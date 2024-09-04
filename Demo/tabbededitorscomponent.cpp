
#include "tabbededitorscomponent.hpp"

#include "font-utils.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <sstream>

const int tabBarHeight = 45;

TabbedEditorsComponent::TabbedEditorsComponent(std::unique_ptr<Font> &font)
    : _font(font)
{
}

bool TabbedEditorsComponent::init(const glm::vec2 &origin)
{
    _origin = origin;

    tabItemMargin.Bottom = tabItemMargin.Top = 4;
    tabItemMargin.Left = tabItemMargin.Right = 4;

    tabItemPadding.Bottom = tabItemPadding.Top = 2;
    tabItemPadding.Left = tabItemPadding.Right = 10;

    return true;
}

void TabbedEditorsComponent::loadFile(const std::string &fileName)
{
    std::stringstream buffer;

    std::ifstream t(fileName.c_str());
    buffer << t.rdbuf();

    auto editorLayer = std::make_shared<EditorComponent>();
    editorLayer->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));
    editorLayer->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    editorLayer->title = std::filesystem::path(fileName).filename().generic_string();
    editorLayer->loadContent(buffer.str());
    tabs.push_back(std::move(editorLayer));
}

void TabbedEditorsComponent::newTab()
{
    auto editorLayer = std::make_shared<EditorComponent>();
    editorLayer->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));
    editorLayer->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    editorLayer->title = "empty.c";
    tabs.push_back(std::move(editorLayer));
}

glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

scr::Rectangle TabbedEditorsComponent::RenderTab(
    const struct InputState &inputState,
    const std::string &text,
    float &x,
    float &y,
    bool isActiveTab)
{
    float width = WidthText(_font, text.c_str(), text.size());

    float xbase = x + tabItemMargin.Left + tabItemPadding.Left;
    float ybase = y + tabItemMargin.Top + tabItemPadding.Top;

    auto border = GetBorderRectangle(text, x, y);

    bool hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

    if (inputState.mouseX < _origin.x || inputState.mouseX > _origin.x + _width || inputState.mouseY < _origin.y || inputState.mouseY > _origin.y + _height)
    {
        hover = false;
    }

    glBegin(GL_QUADS);
    glColor4f(0.2f, 0.2f, 0.2f, isActiveTab ? 1.0f : hover ? 0.6f
                                                           : 0.4f);
    glVertex2f(border.left, border.top);
    glVertex2f(border.right, border.top);
    glVertex2f(border.right, border.bottom);
    glVertex2f(border.left, border.bottom);
    glEnd();

    DrawTextBase(_font, xbase, ybase + 20.0f, text.c_str(), text.size(), textFore);

    x = xbase + width + tabItemPadding.Right + tabItemMargin.Right;

    return border;
}

void TabbedEditorsComponent::render(
    const struct InputState &inputState)
{
    const float border = 4.0f;

    (void)inputState;

    float x = _origin.x;
    float y = _origin.y;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
    glVertex2f(_origin.x, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glEnd();

    scr::Rectangle activeTabRect;

    for (const auto &tab : tabs)
    {
        auto title = tab->title;
        if (title.empty())
        {
            title = "unnamed";
        }

        bool isActiveTab = tabs[_activeTab] == tab;

        auto border = RenderTab(inputState, tab->title, x, y, isActiveTab);

        if (isActiveTab)
        {
            activeTabRect = border;
        }
    }

    RenderTab(inputState, " + ", x, y, false);

    glBegin(GL_LINE_STRIP);
    glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glVertex2f(activeTabRect.left, _origin.y + tabBarHeight - border);
    glVertex2f(activeTabRect.left, activeTabRect.top);
    glVertex2f(activeTabRect.right, activeTabRect.top);
    glVertex2f(activeTabRect.right, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glEnd();

    glBegin(GL_QUADS);
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + _height);
    glVertex2f(_origin.x, _origin.y + _height);
    glEnd();

    if (!tabs.empty() && tabs.size() > _activeTab)
    {
        tabs[_activeTab]->render(inputState);
    }
}

void TabbedEditorsComponent::resize(int x, int y, int w, int h)
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

    if (!tabs.empty() && tabs.size() > _activeTab)
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

    if (!tabs.empty() && tabs.size() > _activeTab)
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

    if (!tabs.empty() && tabs.size() > _activeTab)
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

    if (event.x < _origin.x || event.x > _origin.x + _width || event.y < _origin.y || event.y > _origin.y + _height)
    {
        return false;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        float x = _origin.x, y = _origin.y;

        for (size_t i = 0; i < tabs.size(); i++)
        {
            if (tabs[i]->isHit(glm::vec2(event.x, event.y)) && _activeTab == i)
            {
                IComponent::componentWithKeyboardFocus = tabs[i];
            }

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
                    tabs.erase(std::next(tabs.begin(), i));

                    if (_activeTab == i)
                    {
                        _activeTab--;
                    }
                    return true;
                }
            }

            x = border.right;
        }

        auto plusBorder = GetBorderRectangle(" + ", x, y);

        x = plusBorder.right;

        if (plusBorder.Contains(glm::vec2(event.x, event.y)))
        {
            newTab();

            return true;
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

    if (event.x < _origin.x || event.x > _origin.x + _width || event.y < _origin.y || event.y > _origin.y + _height)
    {
        return false;
    }

    if (!tabs.empty() && tabs.size() > _activeTab)
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

    if (!tabs.empty() && tabs.size() > _activeTab)
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
    float width = WidthText(_font, text.c_str(), text.size());

    auto nextx = x + tabItemMargin.Left + tabItemPadding.Left  // Left margin and padding
                 + width                                       // This is the text with
                 + tabItemMargin.Right + tabItemPadding.Right; // Right marignand padding

    scr::Rectangle border;

    border.top = y + tabItemMargin.Top;
    border.bottom = border.top + tabBarHeight;
    border.left = x + tabItemMargin.Left;

    border.right = nextx;

    return border;
}
