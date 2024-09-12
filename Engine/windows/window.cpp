#include <engine.hpp>

#define WM_USER_SHOW_DIALOG (WM_USER + 0)

#define CLASS_NAME "wc_Engine"
#define JOIN(A,B) A ## B
#define W(S) JOIN(L,S)

LRESULT CALLBACK message_callback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg) {
        default: {}
        break; case WM_CLOSE: {
            engine.want_exit = true;
        }
        break; case WM_DESTROY: {
            PostQuitMessage(0);
        }
        break; case WM_USER_SHOW_DIALOG: {
            return MessageBoxA(nullptr, (char const*)lParam, (char const*)wParam, MB_OKCANCEL);
        }
    }
    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

void create_window()
{
    WNDCLASSW window_class{
        .style = 0,
        .lpfnWndProc = message_callback,
        .hInstance = GetModuleHandle(nullptr),
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = (HBRUSH)COLOR_ACTIVEBORDER, // GetStockObject(WHITE_BRUSH)
        .lpszClassName = W(CLASS_NAME),
    };

    RegisterClassW(&window_class);

    engine.window = CreateWindowExW(
        0,
        W(CLASS_NAME),
        W(__TITLE__),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1600,
        900,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
}