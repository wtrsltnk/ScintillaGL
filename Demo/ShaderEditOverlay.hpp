#pragma once

#include "icomponent.hpp"
#include "localmenuitem.hpp"
#include "menucomponent.hpp"
#include "splittercomponent.hpp"
#include <SDL.h>
#include <assert.h>
#include <glad/glad.h>
#include <memory>
#include <vector>

extern struct stbtt_Font defaultFont;
extern struct stbtt_Font iconFont;

class ShaderEditOverlay
{
public:
    ShaderEditOverlay();
    ~ShaderEditOverlay();

    void initialise(int w, int h);
    void resize(int w, int h);
    void reset();

    void handleKeyDown(const SDL_KeyboardEvent &event);
    void handleKeyUp(const SDL_KeyboardEvent &event);
    void handleTextInput(SDL_TextInputEvent &event);
    void handleMouseButtonInput(const SDL_MouseButtonEvent &event);
    void handleMouseMotionInput(const SDL_MouseMotionEvent &event);
    void handleMouseWheel(const SDL_MouseWheelEvent &event);

    void renderFullscreen();

private:
    std::unique_ptr<class Font> localFont;
    std::unique_ptr<class Font> localIconFont;

    float _width = 0.0f;
    float _height = 0.0f;
    bool _ctrl = false;
    bool _shift = false;
    bool _alt = false;
    struct InputState _inputState;
    static std::vector<LocalMenuItem> wouterMenu;

    std::shared_ptr<MenuComponent> _menu;
    std::shared_ptr<SplitterComponent> _editors;
    std::vector<std::weak_ptr<IComponent>> _components;

    void UpdateMods(const SDL_KeyboardEvent &event);
};
