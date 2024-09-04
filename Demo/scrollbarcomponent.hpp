#ifndef SCROLLBARLAYER_HPP
#define SCROLLBARLAYER_HPP

#include "icomponent.hpp"
#include <functional>
#include <glm/glm.hpp>

class ScrollBarComponent : public IComponent
{
public:
    virtual ~ScrollBarComponent() = default;

    bool init(const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    std::function<void(int)> onScrollY;
    std::function<void(float &,float &)> getScrollInfo;

private:
    int scrollBarWidth = 15;
    bool _scrolling = false;
    bool _hoverScroll = false;
    int _startValue = 0;
};

#endif // SCROLLBARLAYER_HPP
