
#include "menulayer.hpp"

#include "stbtt_font.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

const int menuRowHeight = 30;
const float maxWidth = 130.0f;

void DrawTextBase(
    Font &font_,
    float xbase,
    float ybase,
    const char *s,
    size_t len,
    glm::vec4 fore)
{
    stbtt_Font *realFont = (stbtt_Font *)font_.GetID();
    glEnable(GL_TEXTURE_2D);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //  assume orthographic projection with units = screen pixels, origin at top left
    glBindTexture(GL_TEXTURE_2D, realFont->ftex);
    glColor4fv((GLfloat *)&fore);
    glBegin(GL_QUADS);
    float x = xbase, y = ybase;
    while (len--)
    {
        if (*s >= 32 && *s < 128)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(realFont->cdata, 512, 512, *s - 32, &x, &y, &q, 1); // 1=opengl,0=old d3d
            // x = floor(x);
            glTexCoord2f(q.s0, q.t0);
            glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0);
            glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1);
            glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1);
            glVertex2f(q.x0, q.y1);
        }
        ++s;
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    // glDisable(GL_BLEND);
}

float WidthText(
    Font &font_,
    const char *s,
    size_t len)
{
    stbtt_Font *realFont = (stbtt_Font *)font_.GetID();
    // TODO: implement proper UTF-8 handling
    float position = 0;
    while (len--)
    {
        int advance, leftBearing;
        stbtt_GetCodepointHMetrics(&realFont->fontinfo, *s++, &advance, &leftBearing);
        position += advance * realFont->scale; // TODO: +Kerning
    }
    return position;
}

MenuLayer::MenuLayer(Font &font) : _font(font) {}

bool MenuLayer::init(const std::vector<LocalMenuItem> &menuItems, const glm::vec2 &origin)
{
    _menuItems = menuItems;
    _origin = origin;

    menuItemMargin.Bottom = menuItemMargin.Top = 4;
    menuItemMargin.Left = menuItemMargin.Right = 8;

    menuItemPadding.Bottom = menuItemPadding.Top = 4;
    menuItemPadding.Left = menuItemPadding.Right = 8;

    return true;
}

void MenuLayer::render(const struct InputState &inputState)
{
    float x = _origin.x;
    float y = _origin.y;
    glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 textForeDisabled = glm::vec4(0.6f, 0.6f, 0.6f, 0.21f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const std::string subMenuPointer(">");
    float subMenuPointerWidth = WidthText(_font, subMenuPointer.c_str(), subMenuPointer.size());

    for (const auto &menuItem : _menuItems)
    {
        auto border = GetBorderRectangle(menuItem, x, y);

        float xbase = x + menuItemMargin.Left + menuItemPadding.Left;
        float ybase = y + menuItemMargin.Top + menuItemPadding.Top;

        bool hover = inputState.mouseY > border.top && inputState.mouseX > border.left &&
                     inputState.mouseY < border.bottom && inputState.mouseX < border.right;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);

        if (hover)
        {
            glColor4f(0.7f, 0.7f, 0.7f, 0.9f);
        }
        else
        {
            glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        }

        glVertex2f(border.left, border.top);
        glVertex2f(border.right, border.top);
        glVertex2f(border.right, border.bottom);
        glVertex2f(border.left, border.bottom);
        glEnd();

        DrawTextBase(_font, xbase, ybase + 14.0f, menuItem.name.c_str(), menuItem.name.size(), menuItem.enabled ? textFore : textForeDisabled);

        if (_direction == scr::Direction::Vertical && !menuItem.subMenu.empty())
        {
            xbase = border.right - menuItemMargin.Right - menuItemPadding.Right - subMenuPointerWidth;
            DrawTextBase(_font, xbase, ybase + 14.0f, subMenuPointer.c_str(), subMenuPointer.size(), menuItem.enabled ? textFore : textForeDisabled);
        }

        if (_direction == scr::Direction::Horizontal)
        {
            x = border.right;
        }
        else if (_direction == scr::Direction::Vertical)
        {
            y += menuRowHeight;
        }
    }

    if (_openSubMenu != nullptr)
    {
        _openSubMenu->render(inputState);
    }

    // for (const auto &openMenu : openMenus)
    // {
    //     x = openMenu.rect.left;
    //     y = openMenu.rect.bottom;

    //     float maxWidth = 130.0f;

    //     for (const auto &subMenuItem : openMenu.menuItem->subMenu)
    //     {
    //         glm::vec4 border;
    //         border.top = y;
    //         border.bottom = border.top + menuRowHeight;
    //         border.left = x;
    //         border.right = x + maxWidth + menuItemMargin.Left + menuItemPadding.Left + menuItemMargin.Right + menuItemPadding.Right;

    //         bool hover = inputState.mouseY > border.top && inputState.mouseX > border.left &&
    //                      inputState.mouseY < border.bottom && inputState.mouseX < border.right;

    //         glEnable(GL_BLEND);
    //         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //         glBegin(GL_QUADS);
    //         if (hover)
    //         {
    //             glColor4f(0.7f, 0.7f, 0.7f, 0.9f);
    //         }
    //         else
    //         {
    //             glColor4f(0.4f, 0.4f, 0.4f, 0.9f);
    //         }

    //         glVertex2f(border.left, border.top);
    //         glVertex2f(border.right, border.top);
    //         glVertex2f(border.right, border.bottom);
    //         glVertex2f(border.left, border.bottom);
    //         glEnd();

    //         glm::vec4 rc;
    //         rc.top = y + menuItemMargin.Top + menuItemPadding.Top;
    //         rc.bottom = rc.top + (float)menuRowHeight;
    //         rc.left = x + menuItemMargin.Left + menuItemPadding.Left;
    //         rc.right = rc.left + maxWidth;

    //         DrawTextBase(rc, _font, rc.top + 14.0f, subMenuItem.name.c_str(), subMenuItem.name.size(), subMenuItem.enabled ? textFore : textForeDisabled);

    //         if (!subMenuItem.subMenu.empty())
    //         {
    //             rc.left = border.right - menuItemMargin.Right - menuItemPadding.Right - subMenuPointerWidth;
    //             DrawTextBase(rc, _font, rc.top + 14.0f, subMenuPointer.c_str(), subMenuPointer.size(), subMenuItem.enabled ? textFore : textForeDisabled);
    //         }

    //         y += menuRowHeight;
    //     }
    // }
}

void MenuLayer::resize(int w, int h)
{
    _width = w;
    _height = h;

    float x = _origin.x;
    float y = _origin.y;
    menuHeight = menuRowHeight;

    for (const auto &menuItem : _menuItems)
    {
        auto border = GetBorderRectangle(menuItem, x, y);

        if (border.right > _width)
        {
            border.right -= x;
            menuHeight += menuRowHeight;
        }

        if (_direction == scr::Direction::Horizontal)
        {
            x = border.right;
        }
        else if (_direction == scr::Direction::Vertical)
        {
            y += menuRowHeight;
        }
    }
}

bool MenuLayer::handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleKeyDown(event, inputState);
    }

    return false;
}

bool MenuLayer::handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleKeyUp(event, inputState);
    }

    return false;
}

bool MenuLayer::handleTextInput(SDL_TextInputEvent &event, const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleTextInput(event, inputState);
    }

    return false;
}

bool MenuLayer::handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState)
{
    if (_openSubMenu != nullptr && _openSubMenu->handleMouseButtonInput(event, inputState))
    {
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        float x = _origin.x;
        float y = _origin.y;

        for (const auto &menuItem : _menuItems)
        {
            auto border = GetBorderRectangle(menuItem, x, y);

            if (border.Contains(glm::vec2(event.x, event.y)))
            {
                if (menuItem.subMenu.empty())
                {
                    std::cout << "click: " << menuItem.name << std::endl;

                    menuItem.action();

                    if (_openSubMenu != nullptr)
                    {
                        _openSubMenu.release();
                        _subMenuParentName = std::string();
                    }
                }
                else if (_subMenuParentName != menuItem.name)
                {
                    _subMenuParentName = menuItem.name;

                    _openSubMenu = std::make_unique<MenuLayer>(_font);
                    if (_direction == scr::Direction::Horizontal)
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.left, border.bottom));
                    }
                    else
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.right, border.top));
                    }
                    _openSubMenu->resize(_width, _height);
                    _openSubMenu->_direction = scr::Direction::Vertical;
                }

                return true;
            }

            if (_direction == scr::Direction::Horizontal)
            {
                x = border.right;
            }
            else if (_direction == scr::Direction::Vertical)
            {
                y += menuRowHeight;
            }
        }

        _openSubMenu.release();
    }

    return false;
}

bool MenuLayer::handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        if (_openSubMenu->handleMouseMotionInput(event, inputState))
        {
            return true;
        }

        float x = _origin.x;
        float y = _origin.y;

        for (const auto &menuItem : _menuItems)
        {
            auto border = GetBorderRectangle(menuItem, x, y);

            if (border.Contains(glm::vec2(event.x, event.y)))
            {
                if (!menuItem.subMenu.empty() && _subMenuParentName != menuItem.name)
                {
                    _subMenuParentName = menuItem.name;

                    _openSubMenu = std::make_unique<MenuLayer>(_font);
                    if (_direction == scr::Direction::Horizontal)
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.left, border.bottom));
                    }
                    else
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.right, border.top));
                    }
                    _openSubMenu->resize(_width, _height);
                    _openSubMenu->_direction = scr::Direction::Vertical;
                }

                return true;
            }

            if (_direction == scr::Direction::Horizontal)
            {
                x = border.right;
            }
            else if (_direction == scr::Direction::Vertical)
            {
                y += menuRowHeight;
            }
        }
    }

    return false;
}

bool MenuLayer::handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

scr::Rectangle MenuLayer::GetBorderRectangle(
    const LocalMenuItem &menuItem,
    float x,
    float y)
{
    float width = WidthText(_font, menuItem.name.c_str(), menuItem.name.size());

    auto nextx = x + menuItemMargin.Left + menuItemPadding.Left  // Left margin and padding
                 + width                                         // This is the text with
                 + menuItemMargin.Right + menuItemPadding.Right; // Right marignand padding

    if (nextx > _width)
    {
        nextx -= x;
        x = _origin.x;
        y += menuRowHeight;
    }

    scr::Rectangle border;

    border.top = y;
    border.bottom = border.top + menuRowHeight;
    border.left = x;

    if (_direction == scr::Direction::Horizontal)
    {
        border.right = nextx;
    }
    else if (_direction == scr::Direction::Vertical)
    {
        border.right = x + maxWidth + menuItemMargin.Left + menuItemPadding.Left + menuItemMargin.Right + menuItemPadding.Right;
    }

    return border;
}
