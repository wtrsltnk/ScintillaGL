#ifndef SPLITTERCOMPONENT_HPP
#define SPLITTERCOMPONENT_HPP

#include "tabbededitorscomponent.hpp"
#include "icomponent.hpp"
#include <glm/glm.hpp>
#include <memory>

class SplitterComponent : public IComponent
{
public:
    SplitterComponent(std::unique_ptr<Font> &font);

    bool init(const glm::vec2 &origin, float splitAt = 0.0f, bool vertical = false);

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

    std::shared_ptr<TabbedEditorsComponent>& ActiveEditor();
private:
    std::unique_ptr<Font> &_font;
    int _width = 0.0f;
    int _height = 0.0f;
    glm::vec2 _origin;
    std::shared_ptr<TabbedEditorsComponent> _editor;
    std::shared_ptr<SplitterComponent> _panel1;
    std::shared_ptr<SplitterComponent> _panel2;
    float _splitAt = 0.5f;
    bool _verticalSplitting = false;

    bool _isSplitMoving = false;
    glm::vec2 _splitMovingStart;

    scr::Rectangle GetSplitBarRect();
};

#endif // SPLITTERCOMPONENT_HPP
