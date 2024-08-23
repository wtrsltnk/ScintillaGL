
#include "EditorEx.hpp"

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

void EditorEx::StartScroll(int value)
{
    startValue = value;
}

void EditorEx::Scroll(int value, int height)
{
    auto a = (pdoc->LinesTotal() / (float)height);

    auto amount = (int)(startValue - value) * a;

    if (amount == 0)
    {
        return;
    }

    ScrollY(amount);
    startValue = value;
}

void EditorEx::Reset()
{
    Command(SCI_CANCEL);
    Command(SCI_CLEARALL);
    Command(SCI_EMPTYUNDOBUFFER);
    Command(SCI_SETFOCUS, true);
    Command(SCI_SETHSCROLLBAR, true);
}
