#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <SDL.h>
#include <glm/glm.hpp>
#include <memory>

struct InputState
{
    int mouseX = 0;
    int mouseY = 0;
    bool leftMouseDown = false;
    bool middleMouseDown = false;
    bool rightMouseDown = false;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
};

class IComponent
{
public:
    virtual ~IComponent() = default;

    virtual void render(const struct InputState &inputState) = 0;
    virtual void resize(int x, int y, int w, int h) = 0;

    virtual int width();
    virtual int height();

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState) = 0;
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState) = 0;
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState) = 0;
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState) = 0;
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState) = 0;
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState) = 0;

    static std::shared_ptr<IComponent> componentWithKeyboardFocus;

    bool isHit(
        const glm::vec2 &p);

protected:
    int _width = 0.0f;
    int _height = 0.0f;
    glm::vec2 _origin;
};

#endif // ICOMPONENT_H
