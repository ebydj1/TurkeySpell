// General Project Goals
// 1. Respond to keyboard input
// 2. Print letters and play sounds to respond to characters
// 3. Draw pictures and play sounds to respond to sequences

// Functional Layout
// WinMain: Top-level entry function. Set up the window. Clean up on program exit.
// ├ InitDictionary: Read the program data files and load them into memory.
// └ WndProc: Top-level window function. Handle custom events in subfunctions.
//   ├ CatchCharacter: Respond to pressed character.
//   └ UpdateWindow: Respond to paint request.

// Global variables
// ├ Window configuration
//   ├ Window name: szWindowClass
//   ├ Window title: szTitle
//   └ Instance handle: hInst
// └ Program state
//   ├ Last string pressed: sSequence
//   ├ Dictionary of valid words: dWords
//   └ Background needs to be redrawn: bClear

// Data types
// └ TurkeyData: Contains an image and a sound per key.

#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <string>
#include <map>

// Forward declarations of functions
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);
void InitDictionary();
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);
void RegisterCharacter(_In_ HWND hWnd, _In_ WPARAM wCode, _In_ LPARAM lFlags);
void DrawScreen(_In_ HWND hWnd);

// Data types
class TurkeyData {
    // Eventually this will have an image and a sound per key.
};

// Global variables
static const TCHAR szWindowClass[] = _T("TurkeySpell");
static const TCHAR szTitle[] = _T("TurkeySpell, by Dad");
HINSTANCE hInst;
#ifdef UNICODE
std::wstring sSequence;
#else
std::string sSequence;
#endif
std::map<std::string, TurkeyData> dWords;
bool bClear;

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

    InitDictionary();

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
    MessageBox(NULL,
        _T("I promise I'll initialize the dictionary someday"),
        _T("Stubby McStub"),
        NULL);
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

    hdc = BeginPaint(hWnd, &ps);

    if (bClear) {
        RECT lpRect;
        GetClientRect(hWnd, &lpRect);
        FillRect(hdc, &lpRect, (HBRUSH)(COLOR_WINDOW+1));
        bClear = false;
    }
    TextOut(hdc, 5, 5,
        sSequence.c_str(), sSequence.length());

    EndPaint(hWnd, &ps);
}

void RegisterCharacter(
    _In_ HWND hWnd,
    _In_ WPARAM wCode,
    _In_ LPARAM lFlags
) {
    if ((UINT)wCode == 13 /* newline */) {
        sSequence = {};
        bClear = true;
    } else if (wCode == 8 /* backspace */) {
        sSequence.pop_back();
        bClear = true;
    } else if (isprint(wCode)) {
        sSequence = sSequence + (TCHAR)wCode;
    }
    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
}