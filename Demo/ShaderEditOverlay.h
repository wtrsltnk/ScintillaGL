#pragma once



#include <new>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>



#include <time.h>


#include <string>
#include <vector>
#include <map>

#include <SDL.h>
#include <glad/glad.h>

#include "Platform.h"

#include "ILexer.h"
#include "Scintilla.h"
#include "SVector.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "KeyMap.h"



#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"





#include "Style.h"
#include "ViewStyle.h"
#include "Decoration.h"
#include "CharClassify.h"


#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "Editor.h"



#include "UniConversion.h"

#include "SciLexer.h"
#include "LexerModule.h"


#include "Catalogue.h"
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

private:
    static const size_t TICK_INTERVAL = 100;

    bool mRequireReset = false;

    size_t mNextTick = 0;

    LexState *mLexer = nullptr;

    Editor mShaderEditor;
    Editor *mActiveEditor = nullptr;

    float mWidth = 0.0f;
    float mHeight = 0.0f;
};
