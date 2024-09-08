
#include "EditorEx.hpp"

void EditorEx::Resize(int x, int y, int w, int h)
{ // clientRect = PRectangle(x, y, w, h);
    SetSize(w, h);
    ContainerNeedsUpdate(SC_UPDATE_H_SCROLL);
    InvalidateStyleRedraw();
    ReconfigureScrollBars();
    caret.period = 2500;
}

void EditorEx::DebugPrint()
{
    std::cout << posTopLine << std::endl;
    std::cout << pdoc->LinesTotal() << std::endl;
    std::cout << LinesOnScreen() << std::endl;
}

void EditorEx::GetScrollBar(float &start, float &length)
{
    start = topLine / static_cast<float>(pdoc->LinesTotal());
    length = LinesOnScreen() / static_cast<float>(pdoc->LinesTotal());
}

void EditorEx::Scroll(int diff, int height)
{
    auto a = (pdoc->LinesTotal() / float(height));

    auto amount = int(diff * a);

    if (amount == 0)
    {
        return;
    }

    ScrollY(amount);
}

void EditorEx::Reset()
{
    Command(SCI_CANCEL);
    Command(SCI_CLEARALL);
    Command(SCI_EMPTYUNDOBUFFER);
    Command(SCI_SETFOCUS, true);
    Command(SCI_SETHSCROLLBAR, true);
}
