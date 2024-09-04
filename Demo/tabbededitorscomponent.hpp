#ifndef TABBEDEDITORSLAYER_HPP
#define TABBEDEDITORSLAYER_HPP

#include "editorcomponent.hpp"
#include "icomponent.hpp"
#include "screen-utils.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class TabbedEditorsComponent : public IComponent
{
public:
    TabbedEditorsComponent(std::unique_ptr<Font> &font);
    virtual ~TabbedEditorsComponent() = default;

    bool init(const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual int width();
    virtual int height();

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    struct scr::Padding tabItemPadding;
    struct scr::Margin tabItemMargin;

    void loadFile(const std::string &fileName);
    void newTab();

private:
    std::unique_ptr<Font> &_font;
    int _width = 0.0f;
    int _height = 0.0f;
    glm::vec2 _origin;
    std::vector<std::shared_ptr<EditorComponent>> _tabs;
    size_t _activeTab = 0;
    bool _draggingTab = false;
    int _draggingStartX = 0;

    scr::Rectangle GetBorderRectangle(
        const std::string &text,
        float &x,
        float &y);

    scr::Rectangle RenderTab(
        const struct InputState &inputState,
        const std::string &text,
        float &x,
        float &y,
        bool isActiveTab);
};

#endif // TABBEDEDITORSLAYER_HPP
