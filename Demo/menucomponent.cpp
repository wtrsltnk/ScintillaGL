
#include "menucomponent.hpp"

#include "screen-utils.hpp"
#include "stbtt_font.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

const int menuRowHeight = 30;
const float maxWidth = 130.0f;

void DrawTextBase(
    std::unique_ptr<Font> &font_,
    float xbase,
    float ybase,
    const std::string &text,
    glm::vec4 fore)
{
    stbtt_Font *realFont = (stbtt_Font *)font_->GetID();
    glEnable(GL_TEXTURE_2D);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //  assume orthographic projection with units = screen pixels, origin at top left
    glBindTexture(GL_TEXTURE_2D, realFont->ftex);
    glColor4fv((GLfloat *)&fore);
    glBegin(GL_QUADS);

    float x = xbase, y = ybase;
    const char *s = text.c_str();
    int len = text.size();
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
    std::unique_ptr<Font> &font_,
    const std::string &text)
{
    const char *s = text.c_str();
    int len = text.size();
    stbtt_Font *realFont = (stbtt_Font *)font_->GetID();
    // TODO: implement proper UTF-8 handling
    float position = 0;
    while (len--)
    {
        int advance, leftBearing;
        if (realFont->fontinfo.data == nullptr)
        {
            return position;
        }

        stbtt_GetCodepointHMetrics(&realFont->fontinfo, *s++, &advance, &leftBearing);

        position += advance * realFont->scale; // TODO: +Kerning
    }
    return position;
}

float WidthIcon(
    std::unique_ptr<Font> &font_,
    const std::string &text)
{
    float x = 0, y = 0;
    const char *s = text.c_str();
    int len = text.size();
    stbtt_Font *realFont = (stbtt_Font *)font_->GetID();

    float position = 0;
    while (len--)
    {
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(realFont->cdata, 512, 512, *s - 32, &x, &y, &q, 1);

        position += (q.x1 - q.x0); // TODO: +Kerning
    }
    return position;
}

MenuComponent::MenuComponent(std::unique_ptr<Font> &font) : _font(font) {}

bool MenuComponent::init(
    const std::vector<LocalMenuItem> &menuItems,
    const glm::vec2 &origin,
    scr::Direction direction)
{
    _menuItems = menuItems;
    _origin = origin;
    _direction = direction;

    menuItemMargin.Bottom = menuItemMargin.Top = 4;
    menuItemMargin.Left = menuItemMargin.Right = 8;

    menuItemPadding.Bottom = menuItemPadding.Top = 4;
    menuItemPadding.Left = menuItemPadding.Right = 8;

    return true;
}

void MenuComponent::render(
    const struct InputState &inputState)
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);

    float x = _origin.x;
    float y = _origin.y;
    glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 textForeDisabled = glm::vec4(0.6f, 0.6f, 0.6f, 0.4f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const std::string subMenuPointer(">");
    float subMenuPointerWidth = WidthText(_font, subMenuPointer);

    scr::Rectangle menuRect;
    menuRect.left = menuRect.right = _origin.x;
    menuRect.top = menuRect.bottom = _origin.y;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto &menuItem : _menuItems)
    {
        auto border = GetBorderRectangle(menuItem, x, y);

        menuRect += border;

        float xbase = x + menuItemMargin.Left + menuItemPadding.Left;
        float ybase = y + menuItemMargin.Top + menuItemPadding.Top;

        bool hover = inputState.mouseY > border.top && inputState.mouseX > border.left &&
                     inputState.mouseY < border.bottom && inputState.mouseX < border.right;

        if (hover)
        {
            scr::FillQuad({0.5f, 0.5f, 0.5f, 1.0f}, border);
        }
        else
        {
            scr::FillQuad({0.4f, 0.4f, 0.4f, 1.0f}, border);
        }

        DrawTextBase(_font, xbase, ybase + 14.0f, menuItem.name, menuItem.enabled ? textFore : textForeDisabled);

        if (_direction == scr::Direction::Vertical && !menuItem.subMenu.empty())
        {
            xbase = border.right - menuItemMargin.Right - menuItemPadding.Right - subMenuPointerWidth;
            DrawTextBase(_font, xbase, ybase + 14.0f, subMenuPointer, menuItem.enabled ? textFore : textForeDisabled);
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

    if (_direction == scr::Direction::Horizontal)
    {
        menuRect.right = menuRect.left + _width;
    }

    if (_direction == scr::Direction::Vertical)
    {
        scr::DrawQuad({0.5f, 0.5f, 0.5f, 1.0f}, menuRect);
    }

    if (_openSubMenu != nullptr)
    {
        _openSubMenu->render(inputState);
    }
}

void MenuComponent::resize(
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

    menuHeight = menuRowHeight;

    float tmpX = float(x);
    float tmpY = float(y);

    for (const auto &menuItem : _menuItems)
    {
        auto border = GetBorderRectangle(menuItem, tmpX, tmpY);

        if (border.right > _width)
        {
            border.right -= tmpX;
        }

        if (_direction == scr::Direction::Horizontal)
        {
            tmpX = border.right;
        }
        else if (_direction == scr::Direction::Vertical)
        {
            tmpY += menuRowHeight;
        }

        menuHeight = border.bottom;
    }
}

int MenuComponent::height()
{
    return int(menuHeight);
}

bool MenuComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleKeyDown(event, inputState);
    }

    return false;
}

bool MenuComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleKeyUp(event, inputState);
    }

    return false;
}

bool MenuComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        return _openSubMenu->handleTextInput(event, inputState);
    }

    return false;
}

bool MenuComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    if (_openSubMenu != nullptr && _openSubMenu->handleMouseButtonInput(event, inputState))
    {
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        _mouseDownOnMenuItem = false;
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
                _mouseDownOnMenuItem = true;
                if (menuItem.subMenu.empty())
                {
                    std::cout << "click: " << menuItem.name << std::endl;

                    menuItem.action();

                    if (_openSubMenu != nullptr)
                    {
                        _openSubMenu = nullptr;
                        _subMenuParentName = std::string();
                    }
                }
                else if (_subMenuParentName != menuItem.name)
                {
                    _subMenuParentName = menuItem.name;

                    _openSubMenu = std::make_shared<MenuComponent>(_font);
                    if (_direction == scr::Direction::Horizontal)
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.left, border.bottom));
                    }
                    else
                    {
                        _openSubMenu->init(menuItem.subMenu, glm::vec2(border.right, border.top));
                    }
                    _openSubMenu->resize(-1, -1, _width, _height);
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

        _subMenuParentName = std::string();
        _openSubMenu = nullptr;
    }

    return false;
}

bool MenuComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    if (_openSubMenu != nullptr)
    {
        if (_openSubMenu->handleMouseMotionInput(event, inputState))
        {
            return true;
        }
    }

    if (!_mouseDownOnMenuItem)
    {
        return false;
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

                _openSubMenu = std::make_shared<MenuComponent>(_font);
                if (_direction == scr::Direction::Horizontal)
                {
                    _openSubMenu->init(menuItem.subMenu, glm::vec2(border.left, border.bottom));
                }
                else
                {
                    _openSubMenu->init(menuItem.subMenu, glm::vec2(border.right, border.top));
                }
                _openSubMenu->resize(-1, -1, _width, _height);
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

    return _mouseDownOnMenuItem;
}

bool MenuComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    return false;
}

scr::Rectangle MenuComponent::GetBorderRectangle(
    const LocalMenuItem &menuItem,
    float &x,
    float &y)
{
    float width = WidthText(_font, menuItem.name);

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
