cmake_minimum_required(VERSION 3.12)

project(ScintillaGL VERSION "0.1.0")

add_executable(ScintillaGL)

target_sources(ScintillaGL
    PRIVATE
        "Demo.cxx"
        "../external/glad.c"
        "DefaultFontData.cpp"
        "ScPlatform.cxx"
        "ShaderEditOverlay.cxx"
        "DefaultFontData.h"
        "ShaderEditOverlay.h"
        "stb_truetype.h"
)

target_compile_features(ScintillaGL
    PRIVATE
        cxx_nullptr
        cxx_std_17
)

target_include_directories(ScintillaGL
    PRIVATE
        "include"
        "../external/include"
        "${PROJECT_BINARY_DIR}"
        "../scintilla"
        "../scintilla/lexlib"
)

target_link_libraries(ScintillaGL
    PRIVATE
        SDL2-static
        scintilla
        winmm.lib
        opengl32.lib
        imm32.lib
)
