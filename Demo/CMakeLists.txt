cmake_minimum_required(VERSION 3.12)

project(Demo VERSION "0.1.0")

add_executable(Demo)

target_sources(Demo
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
        SDL2-static
        scintilla
        winmm.lib
        opengl32.lib
        imm32.lib
)
