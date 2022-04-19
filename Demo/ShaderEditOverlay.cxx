#include <glad/glad.h>
#include <windows.h>

#include "ShaderEditOverlay.h"
#include <SDL.h>

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
                instance = lexCurrent->Create();
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

ShaderEditOverlay::ShaderEditOverlay()
{
    mNextTick = 0;
    mLexer = new LexState(mShaderEditor.GetDocument());
    mShaderEditor.SetLexer(mLexer);
}

ShaderEditOverlay::~ShaderEditOverlay()
{
    delete mLexer;
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
    {SC_MARK_BOXMINUS, SC_MARK_BOXPLUS, SC_MARK_VLINE, SC_MARK_LCORNER, SC_MARK_BOXPLUSCONNECTED, SC_MARK_BOXMINUSCONNECTED, SC_MARK_TCORNER}};

const char *fontName = "c:/windows/fonts/UbuntuMono-Regular.ttf";

void ShaderEditOverlay::initialiseShaderEditor()
{

    mShaderEditor.Command(SCI_SETSTYLEBITS, 7);

    mLexer->SetLexer(SCLEX_CPP);
    mLexer->SetWordList(0, cppKeyword);
    mLexer->PropSet("fold", "0");

    // Set up the global default style. These attributes are used wherever no explicit choices are made.
    SetAStyle(mShaderEditor, STYLE_DEFAULT, 0xFFFFFFFF, 0xD0000000, 20, fontName);
    mShaderEditor.Command(SCI_STYLECLEARALL); // Copies global style to all others
    SetAStyle(mShaderEditor, STYLE_INDENTGUIDE, 0xFFC0C0C0, 0xD0000000, 20, fontName);
    SetAStyle(mShaderEditor, STYLE_BRACELIGHT, 0xFF00FF00, 0xD0000000, 20, fontName);
    SetAStyle(mShaderEditor, STYLE_BRACEBAD, 0xFF0000FF, 0xD0000000, 20, fontName);
    SetAStyle(mShaderEditor, STYLE_LINENUMBER, 0xFFC0C0C0, 0xD0333333, 20, fontName);
    mShaderEditor.Command(SCI_SETFOLDMARGINCOLOUR, 1, 0xD01A1A1A);
    mShaderEditor.Command(SCI_SETFOLDMARGINHICOLOUR, 1, 0xD01A1A1A);
    mShaderEditor.Command(SCI_SETSELBACK, 1, 0xD0CC9966);
    mShaderEditor.Command(SCI_SETCARETFORE, 0xFFFFFFFF, 0);
    mShaderEditor.Command(SCI_SETCARETLINEVISIBLE, 1);
    mShaderEditor.Command(SCI_SETCARETLINEBACK, 0xFFFFFFFF);
    mShaderEditor.Command(SCI_SETCARETLINEBACKALPHA, 0x20);

    mShaderEditor.Command(SCI_SETMARGINWIDTHN, 0, 56);             //Calculate correct width
    mShaderEditor.Command(SCI_SETMARGINWIDTHN, 1, 20);             //Calculate correct width
    mShaderEditor.Command(SCI_SETMARGINMASKN, 1, SC_MASK_FOLDERS); //Calculate correct width

    for (size_t i = 0; i < NB_FOLDER_STATE; i++)
    {
        mShaderEditor.Command(SCI_MARKERDEFINE, markersArray[FOLDER_TYPE][i], markersArray[4][i]);
        mShaderEditor.Command(SCI_MARKERSETBACK, markersArray[FOLDER_TYPE][i], 0xFF6A6A6A);
        mShaderEditor.Command(SCI_MARKERSETFORE, markersArray[FOLDER_TYPE][i], 0xFF333333);
    }

    mShaderEditor.Command(SCI_SETUSETABS, 1);
    mShaderEditor.Command(SCI_SETTABWIDTH, 4);
    mShaderEditor.Command(SCI_SETINDENTATIONGUIDES, SC_IV_REAL);

    SetAStyle(mShaderEditor, SCE_C_DEFAULT, 0xFFFFFFFF, 0xD0000000, 20, fontName);
    SetAStyle(mShaderEditor, SCE_C_WORD, 0xFF0066FF, 0xD0000000);
    SetAStyle(mShaderEditor, SCE_C_WORD2, 0xFFFFFF00, 0xD0000000);
    //WTF??? SetAStyle(SCE_C_GLOBALCLASS, 0xFF0000FF, 0xFF000000);
    SetAStyle(mShaderEditor, SCE_C_PREPROCESSOR, 0xFFC0C0C0, 0xD0000000);
    SetAStyle(mShaderEditor, SCE_C_NUMBER, 0xFF0080FF, 0xD0000000);
    SetAStyle(mShaderEditor, SCE_C_OPERATOR, 0xFF00CCFF, 0xD0000000);
    SetAStyle(mShaderEditor, SCE_C_COMMENT, 0xFF00FF00, 0xD0000000);
    SetAStyle(mShaderEditor, SCE_C_COMMENTLINE, 0xFF00FF00, 0xD0000000);
}

void ShaderEditOverlay::initialise(int w, int h)
{
    mNextTick = 0;

    initialiseShaderEditor();

    mWidth = w;
    mHeight = h;

    float w1 = mWidth, h1 = mHeight;

    mShaderEditor.SetSize(w1, h1);

    mActiveEditor = &mShaderEditor;
    mActiveEditor->Command(SCI_SETFOCUS, true);

    const char *str = "#include <iostream>\n\nint main(int argc, char *argv[])\n{\n}\n";
    mShaderEditor.Command(SCI_ADDTEXT, strlen(str), reinterpret_cast<LPARAM>(str));
}

void ShaderEditOverlay::reset()
{
    mShaderEditor.Command(SCI_CANCEL);
    mShaderEditor.Command(SCI_CLEARALL);
    mShaderEditor.Command(SCI_EMPTYUNDOBUFFER);
    mShaderEditor.Command(SCI_SETFOCUS, true);
}

void BraceMatch(Editor &ed)
{
    int braceAtCaret = -1;
    int braceOpposite = -1;
    int caretPos = int(ed.Command(SCI_GETCURRENTPOS));
    char charBefore = '\0';

    int lengthDoc = int(ed.Command(SCI_GETLENGTH));

    if ((lengthDoc > 0) && (caretPos > 0))
    {
        charBefore = TCHAR(ed.Command(SCI_GETCHARAT, caretPos - 1, 0));
    }
    // Priority goes to character before caret
    if (charBefore && strchr("[](){}", charBefore))
    {
        braceAtCaret = caretPos - 1;
    }

    if (lengthDoc > 0 && (braceAtCaret < 0))
    {
        // No brace found so check other side
        TCHAR charAfter = TCHAR(ed.Command(SCI_GETCHARAT, caretPos, 0));
        if (charAfter && strchr("[](){}", charAfter))
        {
            braceAtCaret = caretPos;
        }
    }
    if (braceAtCaret >= 0)
        braceOpposite = int(ed.Command(SCI_BRACEMATCH, braceAtCaret, 0));

    if ((braceAtCaret != -1) && (braceOpposite == -1))
    {
        ed.Command(SCI_BRACEBADLIGHT, braceAtCaret);
        ed.Command(SCI_SETHIGHLIGHTGUIDE, 0);
    }
    else
    {
        ed.Command(SCI_BRACEHIGHLIGHT, braceAtCaret, braceOpposite);

        //if (_pEditView->isShownIndentGuide())
        {
            int columnAtCaret = int(ed.Command(SCI_GETCOLUMN, braceAtCaret));
            int columnOpposite = int(ed.Command(SCI_GETCOLUMN, braceOpposite));
            ed.Command(SCI_SETHIGHLIGHTGUIDE, (columnAtCaret < columnOpposite) ? columnAtCaret : columnOpposite);
        }
    }
}

void ShaderEditOverlay::renderFullscreen()
{
    //update logic
    if (timeGetTime() > mNextTick)
    {
        mShaderEditor.Tick();
        mNextTick = timeGetTime() + TICK_INTERVAL;
    }

    BraceMatch(mShaderEditor);

    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, mWidth, 0, mHeight, 0, 500);
    glTranslatef(0, mHeight, 0);
    glScalef(1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);

    glTranslatef(0.0f, 0, 0);
    mShaderEditor.Paint();

    glPopAttrib();
}

bool isModEnabled(int mod, int modState)
{
    int mask = ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE | mod);
    return (modState & mask) == 0 && ((modState & mod) != 0 || mod == 0);
}

void ShaderEditOverlay::handleKeyDown(
    SDL_KeyboardEvent &event)
{
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
        //case SDLK_LSUPER:			sciKey = SCK_WIN;	        break;
        //case SDLK_RSUPER:			sciKey = SCK_RWIN;	        break;
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
        bool ctrlPressed = event.keysym.mod & KMOD_LCTRL || event.keysym.mod & KMOD_RCTRL;
        bool altPressed = event.keysym.mod & KMOD_LALT || event.keysym.mod & KMOD_RALT;
        bool shiftPressed = event.keysym.mod & KMOD_LSHIFT || event.keysym.mod & KMOD_RSHIFT;
        mActiveEditor->KeyDown(
            (SDLK_a <= sciKey && sciKey <= SDLK_z) ? sciKey - 'a' + 'A' : sciKey,
            shiftPressed,
            ctrlPressed,
            altPressed,
            &consumed);
    }
}

void ShaderEditOverlay::handleTextInput(SDL_TextInputEvent &event)
{
    mActiveEditor->AddCharUTF(event.text, strlen(event.text));
}
