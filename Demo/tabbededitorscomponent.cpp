
#include "tabbededitorscomponent.hpp"

#include "filesystemservice.hpp"
#include "font-utils.hpp"
#include "stringhelpers.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <sstream>

const int tabBarHeight = 40;
const char HamburgerButtonText[] = {32, 0};
const char BackButtonText[] = {33, 0};
const char NextButtonText[] = {34, 0};
const char CloseButtonText[] = {35, 0};
const char AddFileButtonText[] = {36, 0};
const char AddFolderButtonText[] = {37, 0};

glm::vec4 textFore = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 textForeDisabled = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);

TabbedEditorsComponent::TabbedEditorsComponent(
    const std::unique_ptr<FileRunnerService> &fileRunnerService,
    std::unique_ptr<Font> &font,
    std::unique_ptr<Font> &iconFont)
    : _fileRunnerService(fileRunnerService), _font(font), _iconFont(iconFont)
{
}

bool TabbedEditorsComponent::init(
    const glm::vec2 &origin)
{
    _origin = origin;

    tabMargin.Bottom = tabMargin.Top = 4;
    tabMargin.Left = 0;
    tabMargin.Right = 2;

    tabPadding.Bottom = 0;
    tabPadding.Top = 2;
    tabPadding.Left = tabPadding.Right = 10;

    fileSystemBrowser = std::make_shared<FileSystemBrowserComponent>(_font);
    fileSystemBrowser->init(glm::vec2(_origin.x, _origin.y + tabBarHeight));

    fileSystemBrowser->resize(_origin.x, _origin.y + tabBarHeight, _width, _height - tabBarHeight);

    fileSystemBrowser->onFileLoad = [&](const std::filesystem::path &fullPath) {
        if (switchedFrom != nullptr)
        {
            auto switchedFromEditor = dynamic_cast<TabbedEditorsComponent *>(switchedFrom.get());
            if (switchedFromEditor != nullptr)
            {
                switchedFromEditor->loadFile(fullPath.generic_string());
                IComponent::componentWithKeyboardFocus = switchedFrom;
            }
            else
            {
                loadFile(fullPath.generic_string());
            }
        }
        else
        {
            loadFile(fullPath.generic_string());
        }
    };

    return true;
}

void TabbedEditorsComponent::loadFile(
    const std::filesystem::path &fileName)
{
    for (size_t i = 0; i < tabs.size(); i++)
    {
        if (tabs[i]->openFile == fileName)
        {
            SelectActiveTab(i);
            return;
        }
    }

    std::stringstream buffer;

    std::ifstream t(fileName.c_str());
    buffer << t.rdbuf();

    auto editorLayer = std::make_shared<EditorComponent>(_fileRunnerService);
    editorLayer->init(glm::vec2(_origin.x, _origin.y + TabRowHeight()));
    editorLayer->resize(_origin.x, _origin.y + TabRowHeight(), _width, _height - TabRowHeight());

    editorLayer->openFile = std::filesystem::path(fileName);
    editorLayer->title = editorLayer->openFile.filename().generic_string();
    editorLayer->loadContent(buffer.str());

    tabs.push_back(std::move(editorLayer));

    SelectActiveTab(tabs.size() - 1);
}

std::shared_ptr<EditorComponent> TabbedEditorsComponent::newTabWithContent(
    const std::string &title,
    bool switchTo,
    const std::string &content)
{
    auto editorLayer = std::make_shared<EditorComponent>(_fileRunnerService);
    editorLayer->init(glm::vec2(_origin.x, _origin.y + TabRowHeight()));
    editorLayer->resize(_origin.x, _origin.y + TabRowHeight(), _width, _height - TabRowHeight());
    editorLayer->title = title;
    editorLayer->loadContent(content);

    tabs.push_back(editorLayer);

    if (switchTo)
    {
        SelectActiveTab(tabs.size() - 1);
    }

    return editorLayer;
}

std::shared_ptr<EditorComponent> TabbedEditorsComponent::newTab(
    const std::string &title,
    bool switchTo)
{
    auto editorLayer = std::make_shared<EditorComponent>(_fileRunnerService);
    editorLayer->init(glm::vec2(_origin.x, _origin.y + TabRowHeight()));
    editorLayer->resize(_origin.x, _origin.y + TabRowHeight(), _width, _height - TabRowHeight());
    editorLayer->title = title;

    tabs.push_back(editorLayer);

    if (switchTo)
    {
        SelectActiveTab(tabs.size() - 1);
    }

    return editorLayer;
}

void TabbedEditorsComponent::closeTab(
    size_t index)
{
    if (tabs.empty())
    {
        return;
    }

    if (index == 0)
    {
        tabs.erase(tabs.begin());
    }
    else
    {
        tabs.erase(std::next(tabs.begin(), index));
    }

    // Make sure all indices in the history are still valid
    for (size_t i = 0; i < _activeTabHistory.size(); i++)
    {
        if (_activeTabHistory[i] == index)
        {
            _activeTabHistory.erase(_activeTabHistory.begin() + i);
            i--;
        }
        else if (_activeTabHistory[i] > index)
        {
            _activeTabHistory[i]--;
        }
    }

    if (_activeTab >= index && _activeTab > 0)
    {
        _activeTab--;
    }

    if (tabs.empty())
    {
        newTab("empty.c", true);
    }
}

void TabbedEditorsComponent::nextTab()
{
    if (_activeTabHistory.empty() || _traverseBackOnTabHistory == 0) return;

    _traverseBackOnTabHistory--;

    _activeTab = _activeTabHistory[_activeTabHistory.size() - 1 - _traverseBackOnTabHistory];
}

void TabbedEditorsComponent::prevTab()
{
    if (_activeTabHistory.empty()) return;

    _traverseBackOnTabHistory++;

    if (_traverseBackOnTabHistory > _activeTabHistory.size() - 1)
    {
        _traverseBackOnTabHistory = _activeTabHistory.size() - 1;
    }

    _activeTab = _activeTabHistory[_activeTabHistory.size() - 1 - _traverseBackOnTabHistory];
}

void TabbedEditorsComponent::finishTabSwitch()
{
    if (_traverseBackOnTabHistory != 0)
    {
        _activeTabHistory.erase(std::next(_activeTabHistory.end(), -int(_traverseBackOnTabHistory + 1)));

        SelectActiveTab(_activeTab);

        _traverseBackOnTabHistory = 0;
    }
}

void TabbedEditorsComponent::RenderTab(
    const struct InputState &inputState,
    const std::string &text,
    float &x,
    float &y,
    bool isActiveTab)
{
    auto border = GetBorderRectangle(text, x, y);

    bool hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        hover = false;
    }

    auto alpha = isActiveTab ? 1.0f : (hover ? 0.6f : 0.4f);
    auto gray = 0.2f;

    scr::FillQuad({gray, gray, _controlMode ? 0.3f : gray, alpha}, border);

    DrawTextBase(
        _font,
        border.left + tabMargin.Left + tabPadding.Left,
        border.top + tabMargin.Top + tabPadding.Top + 20.0f,
        text,
        textFore);

    x = border.right + tabMargin.Right;
}

void TabbedEditorsComponent::RenderIconButton(
    const struct InputState &inputState,
    const std::string &text,
    float &x,
    float &y,
    int mode)
{
    auto border = GetBorderSquare(text, x, y);
    auto iconWidth = WidthIcon(_iconFont, text);

    bool hover = border.Contains(glm::vec2(inputState.mouseX, inputState.mouseY));

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        hover = false;
    }

    auto alpha = hover ? 0.6f : 0.0f;
    auto gray = 0.2f;

    if (mode == 2)
    {
        alpha = 1.0f;
    }

    scr::FillQuad({gray, gray, _controlMode ? 0.3f : gray, alpha}, border);

    DrawTextBase(
        _iconFont,
        border.left + tabMargin.Left + tabPadding.Left + (((tabBarHeight * 0.6) - iconWidth) / 2.0f),
        border.top + tabMargin.Top + tabPadding.Top + 20.0f,
        text,
        mode == 1 ? textForeDisabled : textFore);

    x = border.right + tabMargin.Right;
}

void TabbedEditorsComponent::render(
    const struct InputState &inputState)
{
    (void)inputState;

    if (_closeHamburgerMenu)
    {
        hamburgerMenu = nullptr;
        _closeHamburgerMenu = false;
    }

    float x = _origin.x;
    float y = _origin.y;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderIconButton(inputState, HamburgerButtonText, x, y);

    if (!tabs.empty())
    {
        RenderIconButton(inputState, BackButtonText, x, y, _activeTab == 0);

        RenderIconButton(inputState, NextButtonText, x, y, _activeTab == tabs.size() - 1);
    }
    else
    {
        RenderIconButton(inputState, AddFileButtonText, x, y);

        RenderIconButton(inputState, AddFolderButtonText, x, y);
    }

    if (!tabs.empty())
    {
        if (IComponent::componentWithKeyboardFocus.get() == this)
        {
            tabs[_activeTab]->tick();
        }

        for (const auto &tab : tabs)
        {
            auto title = tab->title;
            if (title.empty())
            {
                title = "unnamed";
            }

            bool isActiveTab = tabs[_activeTab] == tab;

            RenderTab(inputState, tab->title, x, y, isActiveTab);

            if (isActiveTab)
            {
                x -= tabMargin.Left;
                x -= tabMargin.Right;
                x -= tabPadding.Left;
                RenderIconButton(inputState, CloseButtonText, x, y, 2);
            }
        }

        if (!tabs.empty() && tabs.size() > _activeTab)
        {
            tabs[_activeTab]->render(inputState);
        }
    }
    else
    {
        fileSystemBrowser->render(inputState);
    }

    if (hamburgerMenu != nullptr)
    {
        hamburgerMenu->render(inputState);
    }
}

void TabbedEditorsComponent::resize(
    int x,
    int y,
    int w,
    int h)
{
    _width = w;
    _height = h;

    if (x >= 0)
    {
        _origin.x = (float)x;
    }
    else
    {
        x = _origin.x;
    }

    if (y >= 0)
    {
        _origin.y = (float)y;
    }
    else
    {
        y = _origin.y;
    }

    fileSystemBrowser->resize(_origin.x, _origin.y + TabRowHeight(), _width, _height - TabRowHeight());

    for (const auto &tab : tabs)
    {
        tab->resize(_origin.x, _origin.y + TabRowHeight(), _width, _height - TabRowHeight());
    }
}

bool TabbedEditorsComponent::handleKeyDown(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (event.keysym.sym == SDLK_LCTRL || event.keysym.sym == SDLK_RCTRL)
    {
        _controlMode = true;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleKeyDown(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        switch (event.keysym.sym)
        {
            case SDLK_n:
            {
                if (inputState.ctrl)
                {
                    newTab("empty.c", true);

                    return true;
                }
                break;
            }
            case SDLK_f:
            {
                if (inputState.ctrl && inputState.shift)
                {
                    std::stringstream content;

                    content << R"a(//
// Search with C
// change the "needle" and "searchPath" values and
// run the file with alt+x
//
#include <string.h>

#define SHOW_FILE_SIZE 1
#define SHOW_FILE_TIME 2
#define SHOW_FILE_PERMISSIONS 4

// Flags used in the search
int searchFlags = SHOW_FILE_SIZE | SHOW_FILE_TIME;

// The directory to search from
const char* searchPath = ")a";
                    auto root = FileSystem.GetRoot().string();

                    root = escaped(root);
                    content << root;
                    content << R"a(";

// The text that must be in file path to return a file
const char* needle = "test";

// Function that is called when starting a search
const char* init(int *flags)
{
    *flags = searchFlags;

    return searchPath;
}

// Function called for every path found in the search directory to determin
// if the file should return in the results
int filter(const char* path)
{
    return strstr(path, needle) != 0;
}
)a";

                    newTabWithContent("search.c", true, content.str());

                    return true;
                }
                break;
            }
            case SDLK_x:
            {
                if (inputState.alt)
                {
                    auto activeTab = tabs[_activeTab];

                    std::stringstream ss;
                    ss << activeTab->title << " - response";
                    auto addedTab = newTab(ss.str());

                    addedTab->loadContentAsync(activeTab->title, activeTab->getContent());

                    return true;
                }
                break;
            }
            case SDLK_TAB:
            {
                if (_controlMode)
                {
                    if (inputState.shift)
                    {
                        nextTab();
                    }
                    else
                    {
                        prevTab();
                    }
                }
                break;
            }
            case SDLK_F4:
            {
                if (inputState.ctrl)
                {
                    closeTab(_activeTab);
                }
                break;
            }
        }

        return tabs[_activeTab]->handleKeyDown(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleKeyUp(
    const SDL_KeyboardEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (event.keysym.sym == SDLK_LCTRL || event.keysym.sym == SDLK_RCTRL)
    {
        finishTabSwitch();

        _controlMode = false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleKeyUp(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleKeyUp(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleTextInput(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleTextInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (hamburgerMenu != nullptr && hamburgerMenu->handleMouseButtonInput(event, inputState))
    {
        return true;
    }

    if (!isHit(glm::vec2(event.x, event.y)))
    {
        hamburgerMenu = nullptr;

        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseButtonInput(event, inputState))
        {
            return true;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        hamburgerMenu = nullptr;

        float x = _origin.x;
        float y = _origin.y;
        auto mouse = glm::vec2(event.x, event.y);

        auto btn = GetBorderSquare(HamburgerButtonText, x, y);

        if (btn.Contains(mouse))
        {
            std::vector<LocalMenuItem> HamburgerMenuItems = {
                LocalMenuItem("Close all tabs"),
                LocalMenuItem("Add c-file"),
                LocalMenuItem("Add http-file"),
                LocalMenuItem("Add sql-file"),
            };

            HamburgerMenuItems[0].action = [&]() {
                tabs.clear();
                _closeHamburgerMenu = true;
            };

            HamburgerMenuItems[1].action = [&]() {
                newTab("empty.c", true);
                _closeHamburgerMenu = true;
            };

            HamburgerMenuItems[2].action = [&]() {
                newTab("empty.http", true);
                _closeHamburgerMenu = true;
            };

            HamburgerMenuItems[3].action = [&]() {
                newTab("empty.sql", true);
                _closeHamburgerMenu = true;
            };

            hamburgerMenu = std::make_shared<MenuComponent>(_font);
            hamburgerMenu->init(HamburgerMenuItems, glm::vec2(btn.left, btn.bottom), scr::Direction::Vertical);
            hamburgerMenu->resize(btn.left, btn.bottom, _width, _height);

            _closeHamburgerMenu = false;

            return true;
        }

        x = btn.right + tabMargin.Right;

        if (!tabs.empty())
        {
            auto backBtn = GetBorderSquare(BackButtonText, x, y);

            if (backBtn.Contains(mouse))
            {
                SelectActiveTab(_activeTab - 1);

                return true;
            }

            x = backBtn.right + tabMargin.Right;

            auto nextBtn = GetBorderSquare(NextButtonText, x, y);

            if (nextBtn.Contains(mouse))
            {
                SelectActiveTab(_activeTab + 1);

                return true;
            }

            x = nextBtn.right + tabMargin.Right;
        }
        else
        {
            auto addFileBtn = GetBorderSquare(AddFileButtonText, x, y);

            if (addFileBtn.Contains(mouse))
            {

                return true;
            }

            x = addFileBtn.right + tabMargin.Right;

            auto addFolderBtn = GetBorderSquare(AddFolderButtonText, x, y);

            if (addFolderBtn.Contains(mouse))
            {

                return true;
            }

            x = addFolderBtn.right + tabMargin.Right;
        }

        for (size_t i = 0; i < tabs.size(); i++)
        {
            const auto &tab = tabs[i];
            auto border = GetBorderRectangle(tab->title, x, y);

            if (border.Contains(mouse))
            {
                if (event.button == SDL_BUTTON_LEFT)
                {
                    SelectActiveTab(i);

                    _draggingTab = true;
                    _draggingStartX = event.x;

                    return true;
                }
                else if (event.button == SDL_BUTTON_MIDDLE)
                {
                    closeTab(i);
                    return true;
                }
            }

            x = border.right + tabMargin.Right;

            if (tabs[_activeTab] == tab)
            {
                x -= tabMargin.Left;
                x -= tabMargin.Right;
                x -= tabPadding.Left;

                auto closeTabBtn = GetBorderSquare(CloseButtonText, x, y);

                if (closeTabBtn.Contains(mouse))
                {
                    closeTab(i);

                    return true;
                }

                x = closeTabBtn.right + tabMargin.Right;
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        _draggingTab = false;
    }

    if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseButtonInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (hamburgerMenu != nullptr && hamburgerMenu->handleMouseMotionInput(event, inputState))
    {
        return true;
    }

    if (inputState.leftMouseDown && _draggingTab)
    {
        return true;
    }

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseMotionInput(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseMotionInput(event, inputState);
    }

    return false;
}

bool TabbedEditorsComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (tabs.empty())
    {
        if (fileSystemBrowser->handleMouseWheel(event, inputState))
        {
            return true;
        }
    }
    else if (!tabs.empty() && tabs.size() > _activeTab)
    {
        return tabs[_activeTab]->handleMouseWheel(event, inputState);
    }

    return false;
}

void TabbedEditorsComponent::SelectActiveTab(
    size_t tabIndex)
{
    if (tabIndex >= tabs.size())
    {
        return;
    }

    _activeTab = tabIndex;

    _activeTabHistory.push_back(tabIndex);
}

float TabbedEditorsComponent::TabRowHeight() const
{
    return tabMargin.Top + tabPadding.Top // Left margin and padding
           + tabBarHeight                 // This is the tab height
           + tabPadding.Bottom;           // Right padding
}

scr::Rectangle TabbedEditorsComponent::GetBorderRectangle(
    const std::string &text,
    float &x,
    float &y)
{
    float width = WidthText(_font, text);

    scr::Rectangle border;

    border.top = y + tabMargin.Top;
    border.bottom = y + tabMargin.Top + tabPadding.Top // Left margin and padding
                    + tabBarHeight                     // This is the tab height
                    + tabPadding.Bottom;               // Right padding

    border.left = x + tabMargin.Left;
    border.right = x + tabMargin.Left + tabPadding.Left // Left margin and padding
                   + width                              // This is the text with
                   + tabPadding.Right;                  // Right padding

    return border;
}

scr::Rectangle TabbedEditorsComponent::GetBorderSquare(
    const std::string &text,
    float &x,
    float &y)
{
    float width = tabBarHeight * 0.6; // WidthText(_iconFont, text);

    scr::Rectangle border;

    border.top = y + tabMargin.Top;
    border.bottom = y + tabMargin.Top + tabPadding.Top // Left margin and padding
                    + tabBarHeight                     // This is the button height
                    + tabPadding.Bottom;               // Right padding

    border.left = x + tabMargin.Left;
    border.right = x + tabMargin.Left + tabPadding.Left // Left margin and padding
                   + width                              // This is the text with
                   + tabPadding.Right;                  // Right padding

    return border;
}
