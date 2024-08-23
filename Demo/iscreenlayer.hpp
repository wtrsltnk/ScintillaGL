#ifndef ISCREENLAYER_HPP
#define ISCREENLAYER_HPP

#include <SDL.h>

struct InputState
{
    int mouseX = 0;
    int mouseY = 0;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
};

class IScreenLayer
{
public:
    virtual ~IScreenLayer() = default;

    virtual void render(const struct InputState& inputState) = 0;
    virtual void resize(int w, int h) = 0;

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState& inputState) = 0;
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState& inputState) = 0;
    virtual bool handleTextInput(SDL_TextInputEvent &event, const struct InputState& inputState) = 0;
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState& inputState) = 0;
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState& inputState) = 0;
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState& inputState) = 0;
};

#endif // ISCREENLAYER_HPP
