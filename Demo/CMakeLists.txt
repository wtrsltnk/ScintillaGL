cmake_minimum_required(VERSION 3.12)

project(Demo VERSION "0.1.0")

add_executable(Demo)

target_sources(Demo
    PRIVATE
        "../external/glad.c"
        "DefaultFontData.cpp"
        "DefaultFontData.h"
        "Demo.cxx"
        "EditorEx.cpp"
        "EditorEx.hpp"
        "ScPlatform.cxx"
        "ShaderEditOverlay.cpp"
        "ShaderEditOverlay.hpp"
        "editorcomponent.cpp"
        "editorcomponent.hpp"
        "filesystemservice.cpp"
        "filesystemservice.hpp"
        "font-utils.hpp"
        "icomponent.cpp"
        "icomponent.hpp"
        "menucomponent.cpp"
        "menucomponent.hpp"
        "screen-utils.hpp"
        "scrollbarcomponent.cpp"
        "scrollbarcomponent.hpp"
        "splittercomponent.cpp"
        "splittercomponent.hpp"
        "stb_truetype.h"
        "stbtt_font.hpp"
        "tabbededitorscomponent.cpp"
        "tabbededitorscomponent.hpp"
)

target_compile_features(Demo
    PRIVATE
        cxx_nullptr
        cxx_std_17
)

target_include_directories(Demo
    PRIVATE
        "include"
        "../external/include"
        "${PROJECT_BINARY_DIR}"
        "../scintilla"
        "../scintilla/lexlib"
)

target_link_libraries(Demo
    PRIVATE
        glm
        SDL2-static
        scintilla
        winmm.lib
        opengl32.lib
        imm32.lib
)
