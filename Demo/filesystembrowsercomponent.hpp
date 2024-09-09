#ifndef FILESYSTEMBROWSERCOMPONENT_H
#define FILESYSTEMBROWSERCOMPONENT_H

#include "icomponent.hpp"
#include "screen-utils.hpp"
#include "scrollbarcomponent.hpp"
#include <Platform.h>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

class FileSystemBrowserComponent : public IComponent
{
public:
    FileSystemBrowserComponent(std::unique_ptr<Font> &font);

    bool init(const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    struct scr::Padding tabItemPadding;
    struct scr::Margin tabItemMargin;

    std::function<void(const std::filesystem::path &)> onFileLoad;

private:
    std::unique_ptr<Font> &_font;
    std::shared_ptr<ScrollBarComponent> _browserScrollBarFrom;
    std::filesystem::path _relativePathToOpenFolder = "";
    float _totalBrowserLines = 0;
    float _browserTopLine = 0;

    void ScrollY(
        float amount);

    scr::Rectangle GetBorderRectangleForFile(
        float &x,
        float &y);

    void RenderListItem(
        const struct InputState &inputState,
        const std::string &text,
        float &x,
        float &y);

    int ListItemsInView();
};

#endif // FILESYSTEMBROWSERCOMPONENT_H
