#ifndef EDITORLAYER_HPP
#define EDITORLAYER_HPP

#include "icomponent.hpp"
#include "scrollbarcomponent.hpp"
#include <glm/glm.hpp>

#include "EditorEx.hpp"

class EditorComponent : public IComponent
{
public:
    EditorComponent();
    virtual ~EditorComponent() = default;

    bool init(const glm::vec2 &origin);

    virtual void render(const struct InputState &inputState);
    virtual void resize(int x, int y, int w, int h);

    virtual bool handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState);
    virtual bool handleTextInput(const SDL_TextInputEvent &event, const struct InputState &inputState);
    virtual bool handleMouseButtonInput(const SDL_MouseButtonEvent &event, const struct InputState &inputState);
    virtual bool handleMouseMotionInput(const SDL_MouseMotionEvent &event, const struct InputState &inputState);
    virtual bool handleMouseWheel(const SDL_MouseWheelEvent &event, const struct InputState &inputState);

    void loadContent(const std::string &content);

    std::string title;

private:
    ScrollBarComponent _scrollBarLayer;
    EditorEx mMainEditor;

    int _fontSize = 20;

    std::unique_ptr<class LexState> mLexer;

    void initialiseShaderEditor();
};

#endif // EDITORLAYER_HPP
