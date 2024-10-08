#ifndef MENULAYER_HPP
#define MENULAYER_HPP

#include "Platform.h"
#include "icomponent.hpp"
#include "localmenuitem.hpp"
#include "screen-utils.hpp"
#include <memory>
#include <vector>

class MenuComponent : public IComponent
{
public:
    MenuComponent(std::unique_ptr<Font> &font);
    virtual ~MenuComponent() = default;

    bool init(
        const std::vector<LocalMenuItem> &menuItems,
        const glm::vec2 &origin,
        scr::Direction direction = scr::Direction::Horizontal);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual int height();

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

private:
    std::unique_ptr<Font> &_font;
    std::shared_ptr<MenuComponent> _openSubMenu;
    std::string _subMenuParentName;
    bool _mouseDownOnMenuItem = false;

    std::vector<LocalMenuItem> _menuItems;

    float menuHeight = 30.0f;
    struct scr::Padding menuItemPadding;
    struct scr::Margin menuItemMargin;
    scr::Direction _direction = scr::Direction::Horizontal;

    scr::Rectangle GetBorderRectangle(const LocalMenuItem &menuItem, float &x, float &y);
};

#endif // MENULAYER_HPP
