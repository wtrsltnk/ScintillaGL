
#include "tabbededitorslayer.hpp"

#include "font-utils.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <sstream>

const int tabBarHeight = 45;

TabbedEditorsLayer::TabbedEditorsLayer(std::unique_ptr<Font> &font)
    : _font(font)
{
}

bool TabbedEditorsLayer::init(const glm::vec2 &origin)
{
    _origin = origin;

    tabItemMargin.Bottom = tabItemMargin.Top = 4;
    tabItemMargin.Left = tabItemMargin.Right = 4;

    tabItemPadding.Bottom = tabItemPadding.Top = 2;
    tabItemPadding.Left = tabItemPadding.Right = 10;

    return true;
}

void TabbedEditorsLayer::loadFile(const std::string &fileName)
{
    std::stringstream buffer;

    std::ifstream t(fileName.c_str());
    buffer << t.rdbuf();

    auto editorLayer = std::make_unique<EditorLayer>();
    editorLayer->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));
    editorLayer->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    editorLayer->title = std::filesystem::path(fileName).filename().generic_string();
    editorLayer->loadContent(buffer.str());
    _tabs.push_back(std::move(editorLayer));
}

void TabbedEditorsLayer::render(const struct InputState &inputState)
{
    (void)inputState;

    float x = _origin.x;
    float y = _origin.y;
    glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float border = 8.0f;

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
    glVertex2f(_origin.x, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glEnd();

    scr::Rectangle activeTabRect;

    for (const auto &tab : _tabs)
    {
        auto title = tab->title;
        if (title.empty())
        {
            title = "unnamed";
        }

        float width = WidthText(_font, title.c_str(), title.size());

        float xbase = x + tabItemMargin.Left + tabItemPadding.Left;
        float ybase = y + tabItemMargin.Top + tabItemPadding.Top;

        auto border = GetBorderRectangle(tab, x, y);

        bool hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));
        bool isActiveTab = _tabs[_activeTab] == tab;

        if (isActiveTab)
        {
            activeTabRect = border;
        }

        glBegin(GL_QUADS);
        glColor4f(0.2f, 0.2f, 0.2f, isActiveTab ? 1.0f : hover ? 0.6f
                                                                               : 0.4f);
        glVertex2f(border.left, border.top);
        glVertex2f(border.right, border.top);
        glVertex2f(border.right, border.bottom);
        glVertex2f(border.left, border.bottom);
        glEnd();

        DrawTextBase(_font, xbase, ybase + 20.0f, title.c_str(), title.size(), textFore);

        x = xbase + width + tabItemPadding.Right + tabItemMargin.Right;
    }

    glBegin(GL_LINE_STRIP);
    glColor4f(0.3f, 0.0f, 0.0f, 1.0f);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + activeTabRect.left, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + activeTabRect.left, activeTabRect.top);
    glVertex2f(_origin.x + activeTabRect.right, activeTabRect.top);
    glVertex2f(_origin.x + activeTabRect.right, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glEnd();

    glBegin(GL_QUADS);
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
    glVertex2f(_origin.x, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight - border);
    glVertex2f(_origin.x + _width, _origin.y + tabBarHeight);
    glVertex2f(_origin.x, _origin.y + tabBarHeight);
    glEnd();

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        _tabs[_activeTab]->render(inputState);
    }
}

void TabbedEditorsLayer::resize(int x, int y, int w, int h)
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

    for (const auto &tab : _tabs)
    {
        tab->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);
    }
}

int TabbedEditorsLayer::width()
{
    return _width;
}

int TabbedEditorsLayer::height()
{
    return _height;
}

bool TabbedEditorsLayer::handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleKeyDown(event, inputState);
    }

    return false;
}

bool TabbedEditorsLayer::handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleKeyUp(event, inputState);
    }

    return false;
}

bool TabbedEditorsLayer::handleTextInput(SDL_TextInputEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleTextInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsLayer::handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        float x = _origin.x, y = _origin.y;

        for (size_t i = 0; i < _tabs.size(); i++)
        {
            const auto &tab = _tabs[i];
            auto border = GetBorderRectangle(tab, x, y);

            if (border.Contains(glm::vec2(event.x, event.y)))
            {
                if (event.button == SDL_BUTTON_LEFT)
                {
                    _activeTab = i;

                    return true;
                }
                else if (event.button == SDL_BUTTON_MIDDLE)
                {
                    _tabs.erase(std::next(_tabs.begin(), i));

                    if (_activeTab == i)
                    {
                        _activeTab--;
                    }
                    return true;
                }
            }

            x = border.right;
        }
    }

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleMouseButtonInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsLayer::handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleMouseMotionInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsLayer::handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!_tabs.empty() && _tabs.size() > _activeTab)
    {
        return _tabs[_activeTab]->handleMouseWheel(event, inputState);
    }

    return false;
}

scr::Rectangle TabbedEditorsLayer::GetBorderRectangle(
    const std::unique_ptr<EditorLayer> &tab,
    float &x,
    float &y)
{
    float width = WidthText(_font, tab->title.c_str(), tab->title.size());

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
