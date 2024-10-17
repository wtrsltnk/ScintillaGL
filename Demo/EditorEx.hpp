#ifndef EDITOREX_HPP
#define EDITOREX_HPP

#include <iostream>
#include <map>
#include <vector>

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

class EditorEx : public Editor
{
public:
    void Resize(int x, int y, int w, int h);

    void DebugPrint();

    void GetScrollBar(
        float &start,
        float &length);

    void Scroll(
        int value,
        int height);

    void Reset();

    void DoubleClickWord(int x, int y);
};

#endif // EDITOREX_HPP
