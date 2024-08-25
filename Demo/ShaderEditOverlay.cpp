#include <glad/glad.h>
#include <windows.h>

#include "editorlayer.hpp"
#include "menulayer.hpp"

#include "ShaderEditOverlay.hpp"
#include <SDL.h>
#include <fstream>
#include <sstream>

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

    _menuLayer = std::make_shared<MenuLayer>(localFont);
    _layers.push_back(_menuLayer);
    _menuLayer->init(wouterMenu, glm::vec2(0.0f));
    _menuLayer->resize(0, 0, w, h);

    _editorLayer = std::make_shared<EditorLayer>();
    _layers.push_back(_editorLayer);
    _editorLayer->init(glm::vec2(0.0f, _menuLayer->height()));
    _editorLayer->resize(0.0f, _menuLayer->height(), w, h);

    _width = w;
    _height = h;

    loadFile();
}

void ShaderEditOverlay::resize(int w, int h)
{
    _width = w;
    _height = h;

    int y = 0;
    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            layer->resize(0, y, w, h);
            y += layer->height();
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

    for (auto layerPtr = _layers.rbegin(); layerPtr != _layers.rend(); ++layerPtr)
    {
        if (auto layer = layerPtr->lock())
        {
            glPushMatrix();
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            {
                layer->render(_inputState);
            }
            glPopAttrib();
            glPopMatrix();
        }
    }
}

void ShaderEditOverlay::loadFile()
{
    std::stringstream buffer;

    std::ifstream t("C:/Code/small-apps/ScintillaGL/Demo/ShaderEditOverlay.cpp");
    buffer << t.rdbuf();

    _editorLayer->loadContent(buffer.str());
}

void ShaderEditOverlay::handleKeyDown(
    const SDL_KeyboardEvent &event)
{
    UpdateMods(event);

    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            if (layer->handleKeyDown(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleKeyUp(
    const SDL_KeyboardEvent &event)
{
    UpdateMods(event);

    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            if (layer->handleKeyDown(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleTextInput(
    SDL_TextInputEvent &event)
{
    if (_editorLayer->handleTextInput(event, _inputState)) return;
}

void ShaderEditOverlay::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event)
{

    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            if (layer->handleMouseButtonInput(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event)
{
    _inputState.mouseX = event.x;
    _inputState.mouseY = event.y;

    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            if (layer->handleMouseMotionInput(event, _inputState)) return;
        }
    }
}

void ShaderEditOverlay::handleMouseWheel(
    const SDL_MouseWheelEvent &event)
{

    for (auto &layerPtr : _layers)
    {
        if (auto layer = layerPtr.lock())
        {
            if (layer->handleMouseWheel(event, _inputState)) return;
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
