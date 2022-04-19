// A simple demonstration application using Scintilla
#define SDL_MAIN_HANDLED

#include "ShaderEditOverlay.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <iostream>
#include <stdio.h>

static ShaderEditOverlay app;

void Platform_Initialise(HWND hWnd);
void Platform_Finalise();

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) // Init The SDL Library, The VIDEO Subsystem
    {
        return 0; // Get Out Of Here. Sorry.
    }

    auto window = SDL_CreateWindow("ScintillaGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL);
    if (window == 0)
    {
        std::cout << "Failed to create SDL2 window" << std::endl;

        SDL_Quit();

        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    auto context = SDL_GL_CreateContext(window);
    if (context == NULL)
    {
        std::cout << "Failed to create SDL2 GL context" << std::endl;

        SDL_Quit();

        return 1;
    }

    SDL_GL_MakeCurrent(window, context);

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;

        SDL_Quit();

        return 1;
    }

    SDL_SysWMinfo systemInfo;
    SDL_VERSION(&systemInfo.version);

    if (SDL_GetWindowWMInfo(window, &systemInfo) != 1)
    {
        return 1;
    }

    Platform_Initialise(systemInfo.info.win.window);

    app.initialise(1024, 768);

    Scintilla_LinkLexers();

    bool run = true;

    while (run)
    {
        SDL_Event E;
        while (SDL_PollEvent(&E))
        {
            if (E.type == SDL_QUIT)
                run = false;
            else if (E.type == SDL_TEXTINPUT)
            {
                app.handleTextInput(E.text);
            }
            else if (E.type == SDL_KEYDOWN)
            {
                if (E.key.keysym.sym == SDLK_ESCAPE) run = false;

                app.handleKeyDown(E.key);
            }
        }

        glClearColor(0.08f, 0.18f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        app.renderFullscreen();

        SDL_GL_SwapWindow(window);
    }

    Platform_Finalise();

    SDL_Quit();

    return 0;
}
