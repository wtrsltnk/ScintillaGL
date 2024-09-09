#include <glad/glad.h>
#include <windows.h>

#include "ShaderEditOverlay.hpp"
#include <SDL.h>

#include "Platform.h"

ShaderEditOverlay::ShaderEditOverlay() = default;

ShaderEditOverlay::~ShaderEditOverlay() = default;

std::vector<LocalMenuItem> ShaderEditOverlay::wouterMenu = {
    LocalMenuItem(
        "File",
        std::vector<LocalMenuItem>({
            LocalMenuItem("New"),
            LocalMenuItem("Open"),
        })),
    LocalMenuItem(
        "Edit",
        std::vector<LocalMenuItem>({
            LocalMenuItem("Undo", false),
            LocalMenuItem("Redo", false),
            LocalMenuItem(
                "Advanced",
                std::vector<LocalMenuItem>({
                    LocalMenuItem("Sort Lines"),
                    LocalMenuItem("Fold"),
                })),
        })),
    LocalMenuItem("View"),
    LocalMenuItem("Build"),
    LocalMenuItem("Debug"),
    LocalMenuItem("Analyze"),
    LocalMenuItem("Tools"),
    LocalMenuItem("Window"),
    LocalMenuItem("Help"),
};

void ShaderEditOverlay::initialise(int w, int h)
{
    const char *fontName = "C:\\Windows\\Fonts\\consola.ttf";

    FontParameters fp(fontName, 16, 400, false, 0, 0);
    localFont = std::make_unique<Font>();
    localFont->Create(fp);

    _menu = std::make_shared<MenuComponent>(localFont);
    _components.push_back(_menu);
    _menu->init(wouterMenu, glm::vec2(0.0f));

    _editors = std::make_shared<SplitterComponent>(localFont);
    _components.push_back(_editors);
    _editors->init(glm::vec2(0.0f, _menu->height()), 0.3f, true);

    resize(w, h);

    auto &editor = _editors->ActiveEditor();

    if (editor != nullptr)
    {
        editor->newTab(true);
    }
}

void ShaderEditOverlay::resize(int w, int h)
{
    _width = w;
    _height = h;

    int y = 0;
    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            component->resize(0, y, w, h - y);
            y += component->height();
        }
    }
}

void ShaderEditOverlay::renderFullscreen()
{
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, _width, 0, _height, 0, 500);
    glTranslatef(0, _height, 0);
    glScalef(1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (auto componentPtr = _components.rbegin(); componentPtr != _components.rend(); ++componentPtr)
    {
        if (auto component = componentPtr->lock())
        {
            glPushMatrix();
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            {
                component->render(_inputState);
            }
            glPopAttrib();
            glPopMatrix();
        }
    }
}

void ShaderEditOverlay::handleKeyDown(
    const SDL_KeyboardEvent &event)
{
    UpdateMods(event);

    if (IComponent::componentWithKeyboardFocus != nullptr)
    {
        IComponent::componentWithKeyboardFocus->handleKeyDown(event, _inputState);

        return;
    }

    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            if (component->handleKeyDown(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleKeyUp(
    const SDL_KeyboardEvent &event)
{
    UpdateMods(event);

    if (IComponent::componentWithKeyboardFocus != nullptr)
    {
        IComponent::componentWithKeyboardFocus->handleKeyUp(event, _inputState);

        return;
    }

    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            if (component->handleKeyUp(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleTextInput(
    SDL_TextInputEvent &event)
{
    if (IComponent::componentWithKeyboardFocus != nullptr)
    {
        IComponent::componentWithKeyboardFocus->handleTextInput(event, _inputState);

        return;
    }

    auto &editor = _editors->ActiveEditor();

    if (editor != nullptr)
    {
        editor->handleTextInput(event, _inputState);
    }
}

void ShaderEditOverlay::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _inputState.leftMouseDown = event.type == SDL_MOUSEBUTTONDOWN;
    }

    if (event.button == SDL_BUTTON_MIDDLE)
    {
        _inputState.middleMouseDown = event.type == SDL_MOUSEBUTTONDOWN;
    }

    if (event.button == SDL_BUTTON_RIGHT)
    {
        _inputState.rightMouseDown = event.type == SDL_MOUSEBUTTONDOWN;
    }

    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            if (component->handleMouseButtonInput(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event)
{
    _inputState.mouseX = event.x;
    _inputState.mouseY = event.y;

    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            if (component->handleMouseMotionInput(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleMouseWheel(
    const SDL_MouseWheelEvent &event)
{
    for (auto &componentPtr : _components)
    {
        if (auto component = componentPtr.lock())
        {
            if (component->handleMouseWheel(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::UpdateMods(
    const SDL_KeyboardEvent &event)
{
    switch (event.keysym.sym)
    {
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        {
            _inputState.alt = event.keysym.mod & KMOD_LALT || event.keysym.mod & KMOD_RALT;
            _inputState.ctrl = event.keysym.mod & KMOD_LCTRL || event.keysym.mod & KMOD_RCTRL;
            _inputState.shift = event.keysym.mod & KMOD_LSHIFT || event.keysym.mod & KMOD_RSHIFT;
            break;
        }
    }
}
