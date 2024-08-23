#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <SDL.h>
#include <glad/glad.h>

#include "EditorEx.hpp"

#include "menulayer.hpp"

extern struct stbtt_Font defaultFont;

class LexState;





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

    void loadFile();

    void renderFullscreen();

    bool requireReset()
    {
        bool prevValue = mRequireReset;
        mRequireReset = false;
        return prevValue;
    }

private:
    void initialiseShaderEditor();

    static const size_t TICK_INTERVAL = 100;

    bool mRequireReset = false;

    size_t mNextTick = 0;

    LexState *mLexer = nullptr;

    EditorEx mMainEditor;
    EditorEx *mActiveEditor = nullptr;

    float _width = 0.0f;
    float _height = 0.0f;
    bool _ctrl = false;
    bool _shift = false;
    bool _alt = false;
    bool _scrolling = false;
    bool _hoverScroll = false;
    int _fontSize = 20;
    struct InputState _inputState;
    static std::vector<LocalMenuItem> wouterMenu;

    struct
    {
        int menuHeight = 30;
        int sideBarWidth = 0;
        int scrollBarWidth = 15;
    } sizes;

    std::unique_ptr<MenuLayer> _menuLayer;

    void UpdateMods(const SDL_KeyboardEvent &event);

    void ScrollBarRender();
    bool ScrollBarHandleKeyDown(const SDL_KeyboardEvent &event);
    bool ScrollBarHandleKeyUp(const SDL_KeyboardEvent &event);
    bool ScrollBarHandleMouseButtonInput(const SDL_MouseButtonEvent &event);
    bool ScrollBarHandleMouseMotionInput(const SDL_MouseMotionEvent &event);
    bool ScrollBarHandleMouseWheel(const SDL_MouseWheelEvent &event);

    void EditorRender();
    bool EditorHandleKeyDown(const SDL_KeyboardEvent &event);
    bool EditorHandleKeyUp(const SDL_KeyboardEvent &event);
    bool EditorHandleMouseButtonInput(const SDL_MouseButtonEvent &event);
    bool EditorHandleMouseMotionInput(const SDL_MouseMotionEvent &event);
    bool EditorHandleMouseWheel(const SDL_MouseWheelEvent &event);
};
