#ifndef EDITORLAYER_HPP
#define EDITORLAYER_HPP

#include "icomponent.hpp"
#include "scrollbarcomponent.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <mutex>

#include "EditorEx.hpp"
#include "filerunnerservice.hpp"

class EditorComponent : public IComponent
{
public:
    EditorComponent(
        const std::unique_ptr<FileRunnerService> &fileRunnerService);

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

    void loadContent(
        const std::string &content);

    void loadContentAsync(
        const std::string &title,
        const std::string &content);

    std::string getContent();

    void tick() { mMainEditor.Tick(); }
    bool isUnTouched();

    std::filesystem::path openFile;
    std::string title;

private:
    const std::unique_ptr<FileRunnerService> &_fileRunnerService;
    ScrollBarComponent _scrollBarLayer;
    EditorEx mMainEditor;
    std::mutex _contentLoadMutex;
    std::string _contentToLoad;
    bool _contentIsLoading = false;

    int _fontSize = 20;

    std::unique_ptr<class LexState> mLexer;

    void initialiseShaderEditor();

    friend void runThread(
        EditorComponent *thiz,
        const std::string &title,
        const std::string &content);
};

#endif // EDITORLAYER_HPP
