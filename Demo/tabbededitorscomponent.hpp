#ifndef TABBEDEDITORSLAYER_HPP
#define TABBEDEDITORSLAYER_HPP

#include "editorcomponent.hpp"
#include "filesystembrowsercomponent.hpp"
#include "icomponent.hpp"
#include "menucomponent.hpp"
#include "screen-utils.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class TabbedEditorsComponent : public IComponent
{
public:
    TabbedEditorsComponent(
        std::unique_ptr<Font> &font,
        std::unique_ptr<Font> &iconFont);

    virtual ~TabbedEditorsComponent() = default;

    bool init(const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    struct scr::Padding tabPadding;
    struct scr::Margin tabMargin;

    void loadFile(
        const std::filesystem::path &fileName);

    void newTab(
        bool switchTo = true);

    void closeTab(
        size_t index);

    std::shared_ptr<IComponent> switchedFrom;
    std::shared_ptr<FileSystemBrowserComponent> fileSystemBrowser;
    std::vector<std::shared_ptr<EditorComponent>> tabs;
    std::shared_ptr<MenuComponent> hamburgerMenu;

private:
    std::unique_ptr<Font> &_font;
    std::unique_ptr<Font> &_iconFont;
    size_t _activeTab = 0;
    bool _draggingTab = false;
    int _draggingStartX = 0;
    bool _closeHamburgerMenu = false;

    scr::Rectangle GetBorderRectangle(
        const std::string &text,
        float &x,
        float &y);

    scr::Rectangle GetBorderSquare(
        const std::string &text,
        float &x,
        float &y);

    void RenderTab(
        const struct InputState &inputState,
        const std::string &text,
        float &x,
        float &y,
        bool isActiveTab);

    void RenderIconButton(
        const struct InputState &inputState,
        const std::string &text,
        float &x,
        float &y);

    float TabRowHeight() const;
};

#endif // TABBEDEDITORSLAYER_HPP
