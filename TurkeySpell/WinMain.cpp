// General Project Goals
// 1. Respond to keyboard input
// 2. Print letters and play sounds to respond to characters
// 3. Draw pictures and play sounds to respond to sequences

// Functional Layout
// WinMain: Top-level entry function. Set up the window. Clean up on program exit.
// ├ InitDictionary: Read the program data files and load them into memory.
// └ WndProc: Top-level window function. Handle custom events in subfunctions.
//   ├ RegisterCharacter: Respond to pressed character.
//   └ UpdateWindow: Respond to paint request.

// Global variables
// ├ Window configuration
// | ├ Window name: szWindowClass
// | ├ Window title: szTitle
// | ├ Instance handle: hInst
// | └ Red brush: hbRed 
// └ Program state
//   ├ Sounds directory: dirSounds
//   ├ Last string pressed: sSequence
//   ├ Dictionary of valid words: dWords
//   ├ Background needs to be redrawn: bClear
//   └ Backspace was pressed: bBack

// Data types
// ├ tstring: A std::string or a std::wstring depending on UNICODE setting.
// └ TurkeyData: Contains an image and a sound per key.

#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <string>
#include <regex>
#include <map>

// Forward declarations of functions
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);
void InitDictionary();
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);
void RegisterCharacter(_In_ HWND hWnd, _In_ WPARAM wCode, _In_ LPARAM lFlags);
void DrawScreen(_In_ HWND hWnd);

// Data types
#ifdef UNICODE
typedef std::wstring tstring;
typedef std::wregex tregex;
#else
typedef std::string tstring;
typedef std::regex tregex;
#endif
class TurkeyData {
private:
    tstring pathToSound;
    tstring pathToPicture;
public:
    TurkeyData(tstring _pathToSound = _T(""),
        tstring _pathToPicture = _T(""))
        : pathToSound(_pathToSound),
        pathToPicture(_pathToPicture) {}
    const tstring& getPathToSound() {
        return pathToSound;
    }
    const tstring& getPathToPicture() {
        return pathToPicture;
    }
};
typedef std::map<tstring, TurkeyData> turkeyMap;

// Global variables
static const TCHAR szWindowClass[] = _T("TurkeySpell");
static const TCHAR szTitle[] = _T("TurkeySpell, by Dad");
static const COLORREF redColor = RGB(255, 127, 127);
HINSTANCE hInst;
HBRUSH hbRed;
HFONT hfBigLetter;
tstring dirSounds;
tstring sSequence;
turkeyMap dWords;
bool bClear;
bool bBack;

// Constants
const TCHAR SOUNDS_SUFFIX[] = _T("\\sounds");

int WINAPI WinMain(
    _In_        HINSTANCE   hInstance,
    _In_opt_    HINSTANCE   hPrevInstance,
    _In_        LPSTR       lpCmdLine,
    _In_        int         nCmdShow
) {
    WNDCLASSEX wcex = {
        .cbSize         = sizeof(WNDCLASSEX),
        .style          = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc    = WndProc,
        .cbClsExtra     = 0,
        .cbWndExtra     = 0,
        .hInstance      = hInstance,
        .hIcon          = LoadIcon(hInstance, IDI_APPLICATION),
        .hCursor        = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName   = NULL,
        .lpszClassName  = szWindowClass,
        .hIconSm        = LoadIcon(hInstance, IDI_APPLICATION)
    };
    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);
        return 1;
    }

    hInst = hInstance;
    sSequence = _T("");

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, // x-y starting location
        500, 100, // x-y size
        NULL, NULL, hInstance, NULL
    );
    if (!hWnd) {
        MessageBox(NULL,
            _T("Call to CreateWindowEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);
        return 1;
    }

    hbRed = CreateSolidBrush(redColor);
    hfBigLetter = CreateFont(36, 0,
        0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FF_DONTCARE,
        NULL);

    InitDictionary();

    bClear = false;
    bBack = false;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void InitDictionary() {
    TCHAR* currdir;
    DWORD currdirlen;

    // Set up dirSounds, which stores the path to the sounds directory.
    currdirlen = GetCurrentDirectory(0, 0);
    currdir = (LPTSTR)malloc(currdirlen * sizeof(TCHAR));
    GetCurrentDirectory(currdirlen, currdir);
    if (currdir != 0) {
        dirSounds += currdir;
    }
    dirSounds += SOUNDS_SUFFIX;
    free(currdir);

    // Find first wav file in directory.
    tstring fileSearchString;
    WIN32_FIND_DATA fileData;
    HANDLE hFindFile;
    fileSearchString = dirSounds + _T("\\*.wav");
    hFindFile = FindFirstFile(fileSearchString.c_str(), &fileData);

    do {
        tstring filePrefix;
        filePrefix = std::regex_replace(tstring(fileData.cFileName), tregex(_T(".wav")), _T(""));
        dWords.insert(std::pair(filePrefix, TurkeyData(dirSounds + _T("\\") + fileData.cFileName)));
    } while (FindNextFile(hFindFile, &fileData));
}

LRESULT CALLBACK WndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
) {
    switch (message) {
    case WM_PAINT:
        DrawScreen(hWnd);
        break;
    case WM_CHAR:
        RegisterCharacter(hWnd, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

void DrawScreen(
    _In_ HWND hWnd
) {
    PAINTSTRUCT ps;
    HDC hdc;
    RECT lpRect;
    HFONT sysDefaultFont;

    hdc = BeginPaint(hWnd, &ps);    
    GetClientRect(hWnd, &lpRect);
    sysDefaultFont = (HFONT)SelectObject(hdc, (HGDIOBJ)hfBigLetter);

    if (bClear && !sSequence.empty()) {
        turkeyMap::iterator it =
            dWords.find(tstring(sSequence.substr(sSequence.length() - 1, sSequence.length())));
        if (!sSequence.empty() && it != dWords.end()) {
            FillRect(hdc, &lpRect, hbRed);
            SetBkColor(hdc, redColor);
            SetTextColor(hdc, RGB(0, 0, 0));
            TextOut(hdc, 5, 5,
                sSequence.c_str(), (int)sSequence.length());
            EndPaint(hWnd, &ps);
            if (!bBack) {
                PlaySound(it->second.getPathToSound().c_str(), NULL, SND_FILENAME | SND_SYNC);
            }
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
        }
    } else {
        FillRect(hdc, &lpRect, (HBRUSH)(COLOR_WINDOW + 1));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        TextOut(hdc, 5, 5,
            sSequence.c_str(), (int)sSequence.length());

        SelectObject(hdc, (HGDIOBJ)sysDefaultFont);
        EndPaint(hWnd, &ps);
    }
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_QS_INPUT));
    bClear = false;
    bBack = false;
}

void RegisterCharacter(
    _In_ HWND hWnd,
    _In_ WPARAM wCode,
    _In_ LPARAM lFlags
) {
    if (!(lFlags & (1 << 30)) && !(lFlags & (1 << 31))) {
        if ((UINT)wCode == 13 /* newline */) {
            sSequence = {};
            bClear = true;
        } else if (wCode == 8 /* backspace */) {
            if (!sSequence.empty()) sSequence.pop_back();
            bClear = true;
            bBack = true;
        } else if (isalpha((int)wCode)) {
            sSequence = sSequence + (TCHAR)tolower((int)wCode);
            bClear = true;
        }
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
    }
}