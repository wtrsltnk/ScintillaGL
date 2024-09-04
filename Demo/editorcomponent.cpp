
#include "editorcomponent.hpp"

#include <glad/glad.h>

class LexState : public LexInterface
{
    const LexerModule *lexCurrent;

public:
    int lexLanguage;

    LexState(Document *pdoc_) : LexInterface(pdoc_)
    {
        lexCurrent = 0;
        performingStyle = false;
        lexLanguage = SCLEX_CONTAINER;
    }

    ~LexState()
    {
        if (instance)
        {
            instance->Release();
            instance = 0;
        }
    }

    void SetLexerModule(const LexerModule *lex)
    {
        if (lex != lexCurrent)
        {
            if (instance)
            {
                instance->Release();
                instance = 0;
            }
            lexCurrent = lex;
            if (lexCurrent)
            {
                instance = lexCurrent->Create();
            }

            pdoc->LexerChanged();
        }
    }

    void SetLexer(uptr_t wParam)
    {
        lexLanguage = wParam;
        if (lexLanguage == SCLEX_CONTAINER)
        {
            SetLexerModule(0);
        }
        else
        {
            const LexerModule *lex = Catalogue::Find(lexLanguage);
            if (!lex)
                lex = Catalogue::Find(SCLEX_NULL);
            SetLexerModule(lex);
        }
    }

    void SetLexerLanguage(const char *languageName)
    {
        const LexerModule *lex = Catalogue::Find(languageName);
        if (!lex)
            lex = Catalogue::Find(SCLEX_NULL);
        if (lex)
            lexLanguage = lex->GetLanguage();
        SetLexerModule(lex);
    }

    void SetWordList(int n, const char *wl)
    {
        if (instance)
        {
            int firstModification = instance->WordListSet(n, wl);
            if (firstModification >= 0)
            {
                pdoc->ModifiedAt(firstModification);
            }
        }
    }

    void PropSet(const char *key, const char *val)
    {
        if (instance)
        {
            int firstModification = instance->PropertySet(key, val);
            if (firstModification >= 0)
            {
                pdoc->ModifiedAt(firstModification);
            }
        }
    }
};

EditorComponent::EditorComponent()
{
    mLexer = std::make_unique<LexState>(mMainEditor.GetDocument());
    mMainEditor.SetLexer(mLexer.get());
}

void EditorComponent::loadContent(const std::string &content)
{
    mMainEditor.Command(SCI_CANCEL);
    mMainEditor.Command(SCI_CLEARALL);
    mMainEditor.Command(SCI_SETUNDOCOLLECTION, 0);
    mMainEditor.Command(SCI_ADDTEXT, content.size() - 1, reinterpret_cast<uptr_t>(content.data()));
    mMainEditor.Command(SCI_SETUNDOCOLLECTION, 1);
    mMainEditor.Command(SCI_EMPTYUNDOBUFFER);
    mMainEditor.Command(SCI_SETSAVEPOINT);
    mMainEditor.Command(SCI_GOTOPOS, 0);
}

bool EditorComponent::init(const glm::vec2 &origin)
{
    _origin = origin;

    _scrollBarLayer.init(glm::vec2(_origin.x, _origin.y));
    _scrollBarLayer.resize(_origin.x, _origin.y, _width, _height);
    _scrollBarLayer.onScrollY = [&](int diff) {
        mMainEditor.Scroll(diff, _height);
    };
    _scrollBarLayer.getScrollInfo = [&](float &start, float &length) {
        mMainEditor.GetScrollBar(start, length);
    };

    initialiseShaderEditor();

    mMainEditor.SetSize(_width, _height);

    mMainEditor.Command(SCI_SETFOCUS, true);

    return true;
}

void EditorComponent::render(const struct InputState &inputState)
{
    (void)inputState;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(_origin.x, _origin.y, 0);

    mMainEditor.Paint();
    glPopMatrix();

    _scrollBarLayer.render(inputState);
}

void EditorComponent::resize(int x, int y, int w, int h)
{
    _width = w;
    _height = h;
    _origin.x = x;
    _origin.y = y;

    mMainEditor.Resize(_origin.x, _origin.y, _width, _height);

    _scrollBarLayer.resize(_origin.x, _origin.y, _width, _height);
}

bool EditorComponent::handleKeyDown(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    if (_scrollBarLayer.handleKeyDown(event, inputState)) return true;

    int sciKey;
    switch (event.keysym.sym)
    {
        case SDLK_DOWN:
            sciKey = SCK_DOWN;
            break;
        case SDLK_UP:
            sciKey = SCK_UP;
            break;
        case SDLK_LEFT:
            sciKey = SCK_LEFT;
            break;
        case SDLK_RIGHT:
            sciKey = SCK_RIGHT;
            break;
        case SDLK_HOME:
            sciKey = SCK_HOME;
            break;
        case SDLK_END:
            sciKey = SCK_END;
            break;
        case SDLK_PAGEUP:
            sciKey = SCK_PRIOR;
            break;
        case SDLK_PAGEDOWN:
            sciKey = SCK_NEXT;
            break;
        case SDLK_DELETE:
            sciKey = SCK_DELETE;
            break;
        case SDLK_INSERT:
            sciKey = SCK_INSERT;
            break;
        case SDLK_ESCAPE:
            sciKey = SCK_ESCAPE;
            break;
        case SDLK_BACKSPACE:
            sciKey = SCK_BACK;
            break;
        case SDLK_TAB:
            sciKey = SCK_TAB;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            sciKey = SCK_RETURN;
            break;
        case SDLK_KP_PLUS:
            sciKey = SCK_ADD;
            break;
        case SDLK_KP_MINUS:
            sciKey = SCK_SUBTRACT;
            break;
        case SDLK_KP_DIVIDE:
            sciKey = SCK_DIVIDE;
            break;
        // case SDLK_LSUPER:			sciKey = SCK_WIN;	        break;
        // case SDLK_RSUPER:			sciKey = SCK_RWIN;	        break;
        case SDLK_MENU:
            sciKey = SCK_MENU;
            break;
        case SDLK_SLASH:
            sciKey = '/';
            break;
        case SDLK_ASTERISK:
            sciKey = '`';
            break;
        case SDLK_LEFTBRACKET:
            sciKey = '[';
            break;
        case SDLK_BACKSLASH:
            sciKey = '\\';
            break;
        case SDLK_RIGHTBRACKET:
            sciKey = ']';
            break;
        case SDLK_0:
            sciKey = '0';
            if (inputState.ctrl)
            {
                _fontSize = 20;
                initialiseShaderEditor();
            }
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            sciKey = 0;
            break;
        default:
            sciKey = event.keysym.sym;
    }

    if (sciKey)
    {
        bool consumed;
        mMainEditor.KeyDown(
            (SDLK_a <= sciKey && sciKey <= SDLK_z) ? sciKey - 'a' + 'A' : sciKey,
            inputState.shift,
            inputState.ctrl,
            inputState.alt,
            &consumed);
    }

    return false;
}

bool EditorComponent::handleKeyUp(const SDL_KeyboardEvent &event, const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_scrollBarLayer.handleKeyUp(event, inputState)) return true;

    return false;
}

bool EditorComponent::handleTextInput(
    const SDL_TextInputEvent &event,
    const struct InputState &inputState)
{
    (void)event;
    (void)inputState;

    if (_scrollBarLayer.handleTextInput(event, inputState)) return true;

    char buffer[SDL_TEXTINPUTEVENT_TEXT_SIZE] = {0};
    strcpy_s(buffer, SDL_TEXTINPUTEVENT_TEXT_SIZE, event.text);

    mMainEditor.AddCharUTF(buffer, strlen(buffer));

    return false;
}

bool EditorComponent::handleMouseButtonInput(
    const SDL_MouseButtonEvent &event,
    const struct InputState &inputState)
{
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (_scrollBarLayer.handleMouseButtonInput(event, inputState)) return true;

    if (event.state == SDL_PRESSED)
    {
        mMainEditor.StartSelectionxy(event.x - _origin.x, event.y - _origin.y);
    }

    return false;
}

bool EditorComponent::handleMouseMotionInput(
    const SDL_MouseMotionEvent &event,
    const struct InputState &inputState)
{
    (void)inputState;

    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (_scrollBarLayer.handleMouseMotionInput(event, inputState)) return true;

    if (event.state == SDL_PRESSED)
    {
        mMainEditor.ChangeSelectionxy(event.x - _origin.x, event.y - _origin.y);
    }

    return false;
}

bool EditorComponent::handleMouseWheel(
    const SDL_MouseWheelEvent &event,
    const struct InputState &inputState)
{
    if (!isHit(glm::vec2(inputState.mouseX, inputState.mouseY)))
    {
        return false;
    }

    if (_scrollBarLayer.handleMouseWheel(event, inputState)) return true;

    (void)event;
    (void)inputState;

    if (inputState.ctrl)
    {
        _fontSize += (event.y * 4);

        if (_fontSize < 8)
        {
            _fontSize = 8;
        }

        if (_fontSize > 72)
        {
            _fontSize = 72;
        }

        initialiseShaderEditor();
    }
    else
    {
        mMainEditor.ScrollY(event.y * 4);
    }

    return false;
}

void SetAStyle(Editor &ed, int style, Colour fore, Colour back = 0xFFFFFFFF, int size = -1, const char *face = 0)
{
    ed.Command(SCI_STYLESETFORE, style, fore);
    ed.Command(SCI_STYLESETBACK, style, back);
    if (size >= 1)
        ed.Command(SCI_STYLESETSIZE, style, size);
    if (face)
        ed.Command(SCI_STYLESETFONT, style, reinterpret_cast<sptr_t>(face));
}

const char cppKeyword[] = {
    "break case const continue default do else enum extern for "
    "goto if inline register restrict return sizeof static struct "
    "switch typedef union volatile while "
    "_Alignas _Alignof _Atomic _Generic _Noreturn _Static_assert _Thread_local "

    "alignas alignof asm catch class consteval constexpr constinit const_cast decltype delete dynamic_cast "
    "explicit export false friend mutable namespace new noexcept noreturn nullptr operator "
    "private protected public reinterpret_cast static_assert static_cast template "
    "this thread_local throw true try typeid typename using virtual "
    "override final "
    "_Pragma defined __has_include __has_include_next __has_attribute __has_c_attribute __has_cpp_attribute "
    "and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq "
    "concept requires audit axiom "
    "import module "
    "co_await co_return co_yield "
    // MSVC
    "__alignof __asm __assume __based __cdecl __clrcall __declspec __event __except __fastcall __finally __forceinline __hook "
    "__identifier __if_exists __if_not_exists __inline __interface __leave __multiple_inheritance __noop __pragma __raise __restrict "
    "__single_inheritance __stdcall __super __thiscall __try __unaligned __unhook __uuidof __vectorcall __virtual_inheritance "
    // COM
    "interface "
    // GCC
    "typeof __typeof__ __alignof__ __label__ __asm__ __thread __attribute__ __volatile__ __restrict__ __inline__ __extension__ "
    // clang Objective-C/C++
    "__nonnull __nullable __covariant __kindof nullable nonnull "
    // Intel
    "__regcall "
    // Keil
    "sfr sfr16 interrupt "

    "NULL TRUE FALSE EOF WEOF "
    "errno "
    "stdin stdout stderr "
    "signgam "
    "CONST ENUM "

};

const char cppKeyword2[] = {
    "__auto_type auto char double float int long short signed unsigned void "
    "bool char8_t char16_t char32_t wchar_t nullptr_t nothrow_t "
    "_Bool complex _Complex _Imaginary imaginary "
    "__w64 __wchar_t __int8 __int16 __int32 __int64 __m64 __m128 __m128d __m128i __m256 __m256d __m256i __m512 __m512d __m512i __mmask8 __mmask16 __mmask32 __mmask64 __int3264 __ptr32 __ptr64 __sptr __uptr "
    "__int128 __float80 __float128 __fp16 __complex__ __real__ __imag__ __complex128 _Decimal32 _Decimal64 _Decimal128 decimal32 decimal64 decimal128 "
    "int128 qfloat "

    // errno.h
    "errno_t "
    // fenv.h
    "fenv_t femode_t fexcept_t "
    // inttypes.h
    "imaxdiv_t "
    // math.h
    "float_t double_t "
    // setjmp.h
    "jmp_buf sigjmp_buf "
    // signal.h
    "sig_atomic_t sigset_t pid_t "
    // stdarg.h
    "va_list "
    // stdatomic.h
    "atomic_flag atomic_bool atomic_char atomic_schar atomic_uchar atomic_short atomic_ushort atomic_int atomic_uint atomic_long atomic_ulong atomic_llong atomic_ullong atomic_char16_t atomic_char32_t atomic_wchar_t atomic_intptr_t atomic_uintptr_t atomic_size_t atomic_ptrdiff_t atomic_intmax_t atomic_uintmax_t "
    // stddef.h
    "size_t rsize_t ptrdiff_t max_align_t "
    // stdint.h
    "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
    "intptr_t uintptr_t intmax_t uintmax_t "
    "ssize_t "
    "locale_t "
    // stdio.h
    "FILE fpos_t off_t "
    // stdlib.h
    "div_t ldiv_t lldiv_t "
    // threads.h
    "cnd_t thrd_t tss_t mtx_t tss_dtor_t thrd_start_t once_flag "
    // time.h
    "clock_t time_t "
    // wchar.h
    "wint_t mbstate_t "
    // wctype.h
    "wctrans_t wctype_t "
    // std::byte
    "byte ",
};

const size_t NB_FOLDER_STATE = 7;
const size_t FOLDER_TYPE = 0;
const int markersArray[][NB_FOLDER_STATE] = {
    {SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERSUB, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND, SC_MARKNUM_FOLDEROPENMID, SC_MARKNUM_FOLDERMIDTAIL},
    {SC_MARK_MINUS, SC_MARK_PLUS, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
    {SC_MARK_ARROWDOWN, SC_MARK_ARROW, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
    {SC_MARK_CIRCLEMINUS, SC_MARK_CIRCLEPLUS, SC_MARK_VLINE, SC_MARK_LCORNERCURVE, SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE},
    {SC_MARK_BOXMINUS, SC_MARK_BOXPLUS, SC_MARK_VLINE, SC_MARK_LCORNER, SC_MARK_BOXPLUSCONNECTED, SC_MARK_BOXMINUSCONNECTED, SC_MARK_TCORNER},
};

const char *fontName = "C:\\Windows\\Fonts\\consola.ttf";

void EditorComponent::initialiseShaderEditor()
{
    mLexer->SetLexer(SCLEX_CPP);
    mLexer->SetWordList(0, cppKeyword);
    mLexer->SetWordList(1, cppKeyword2);
    mLexer->PropSet("fold", "0");

    mMainEditor.Command(SCI_SETSTYLEBITS, 7);

    // Set up the global default style. These attributes are used wherever no explicit choices are made.
    SetAStyle(mMainEditor, STYLE_DEFAULT, 0xFFFFFFFF, 0xFF333333, _fontSize, fontName);
    mMainEditor.Command(SCI_STYLECLEARALL); // Copies global style to all others
    SetAStyle(mMainEditor, STYLE_INDENTGUIDE, 0xFFC0C0C0, 0xFF333333);
    SetAStyle(mMainEditor, STYLE_BRACELIGHT, MakeRGBA(69, 198, 214), 0xFF333333);
    SetAStyle(mMainEditor, STYLE_BRACEBAD, MakeRGBA(69, 198, 214), 0xFF333333);
    SetAStyle(mMainEditor, STYLE_LINENUMBER, 0xFFC0C0C0, 0xFF333333);

    mMainEditor.Command(SCI_SETWRAPMODE, SC_WRAP_WORD);

    mMainEditor.Command(SCI_SETFOLDMARGINCOLOUR, 1, 0xFF333333);
    mMainEditor.Command(SCI_SETFOLDMARGINHICOLOUR, 1, 0xFF333333);
    mMainEditor.Command(SCI_SETSELBACK, 1, 0xD0CC9966);
    mMainEditor.Command(SCI_SETCARETFORE, 0xFFFFFFFF, 0);
    mMainEditor.Command(SCI_SETCARETLINEVISIBLE, 1);
    mMainEditor.Command(SCI_SETCARETLINEBACK, 0xFFFFFFFF);
    mMainEditor.Command(SCI_SETCARETLINEBACKALPHA, 0x0);

    mMainEditor.Command(SCI_SETMARGINWIDTHN, 0, _fontSize * 4);  // Calculate correct width
    mMainEditor.Command(SCI_SETMARGINWIDTHN, 1, 40);             // Calculate correct width
    mMainEditor.Command(SCI_SETMARGINMASKN, 1, SC_MASK_FOLDERS); // Calculate correct width

    for (size_t i = 0; i < NB_FOLDER_STATE; i++)
    {
        mMainEditor.Command(SCI_MARKERDEFINE, markersArray[FOLDER_TYPE][i], markersArray[4][i]);
        mMainEditor.Command(SCI_MARKERSETBACK, markersArray[FOLDER_TYPE][i], 0xFF6A6A6A);
        mMainEditor.Command(SCI_MARKERSETFORE, markersArray[FOLDER_TYPE][i], 0xFF333333);
    }

    mMainEditor.Command(SCI_SETUSETABS, 1);
    mMainEditor.Command(SCI_SETTABWIDTH, 4);
    mMainEditor.Command(SCI_SETINDENTATIONGUIDES, SC_IV_REAL);

    SetAStyle(mMainEditor, SCE_C_DEFAULT, MakeRGBA(214, 207, 154), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_IDENTIFIER, MakeRGBA(214, 207, 154), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_VERBATIM, MakeRGBA(140, 216, 254), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_STRING, MakeRGBA(214, 149, 69), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_WORD, MakeRGBA(69, 198, 214), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_WORD2, MakeRGBA(255, 167, 214), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_GLOBALCLASS, MakeRGBA(154, 167, 214), 0xFF000000); // in keywords 4 list
    SetAStyle(mMainEditor, SCE_C_PREPROCESSOR, MakeRGBA(255, 106, 173), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_NUMBER, MakeRGBA(138, 96, 44), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_OPERATOR, MakeRGBA(214, 187, 154), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_COMMENT, MakeRGBA(168, 171, 176), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_COMMENTLINE, MakeRGBA(168, 171, 176), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_COMMENTLINEDOC, MakeRGBA(168, 171, 176), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_COMMENTDOC, MakeRGBA(168, 171, 176), 0xFF333333);
    SetAStyle(mMainEditor, SCE_C_COMMENTDOCKEYWORDERROR, MakeRGBA(168, 171, 176), 0xFF333333);
}
