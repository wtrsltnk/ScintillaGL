#ifndef MENULAYER_HPP
#define MENULAYER_HPP

#include "Platform.h"
#include "iscreenlayer.hpp"
#include "localmenuitem.hpp"
#include "screen-utils.hpp"
#include <memory>
#include <vector>

class MenuLayer : public IScreenLayer
{
public:
    MenuLayer(Font &font);
    virtual ~MenuLayer() = default;

    bool init(const std::vector<LocalMenuItem> &menuItems, const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int w, int h);

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    std::vector<LocalMenuItem> _menuItems;

    int menuHeight = 30;
    struct scr::Padding menuItemPadding;
    struct scr::Margin menuItemMargin;
    scr::Direction _direction = scr::Direction::Horizontal;

private:
    Font &_font;
    int _width = 0.0f;
    int _height = 0.0f;
    glm::vec2 _origin;
    std::unique_ptr<MenuLayer> _openSubMenu;
    std::string _subMenuParentName;

    scr::Rectangle GetBorderRectangle(const LocalMenuItem &menuItem, float x, float y);
};

#endif // MENULAYER_HPP
