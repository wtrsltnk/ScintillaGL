#ifndef ISCREENLAYER_H
#define ISCREENLAYER_H

#include <SDL.h>

class IScreenLayer
{
public:
    virtual ~IScreenLayer() = default;

    virtual void initialise(int w, int h) = 0;
    virtual void resize(int w, int h) = 0;
    virtual void render() = 0;

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event) = 0;
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event) = 0;
    virtual bool handleTextInput(SDL_TextInputEvent &event) = 0;
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event) = 0;
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event) = 0;
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event) = 0;
};

#endif // ISCREENLAYER_H
