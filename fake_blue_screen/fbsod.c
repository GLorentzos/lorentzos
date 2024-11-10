#include <windows.h>
#include <stdio.h>

#define WINDOW_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define WINDOW_HEIGHT GetSystemMetrics(SM_CYSCREEN)

HHOOK hKeyboardHook;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
    if (wParam == WM_KEYDOWN) {
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (pKeyBoard->vkCode == VK_UP)) {
            PostQuitMessage(0);
            return 1;
        }
        if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (pKeyBoard->vkCode == VK_TAB)) {
            return 1;
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
            FillRect(hdc, &ps.rcPaint, blueBrush);
            DeleteObject(blueBrush);

            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            int baseX = 50;
            int baseY = 50;
            int lineSpacing = 20;

            HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_SWISS, "Consolas");
            SelectObject(hdc, hFont);

            const char* bsodText[] = {
                "A problem has been detected and Windows has been shut down to prevent damage",
                "to your computer.\n",
                "\n",
                "The problem seems to be caused by an unexpected system exception.\n",
                "\n",
                "KERNEL_SECURITY_CHECK_FAILURE\n",
                "\n",
                "If this is the first time you've seen this stop error screen,",
                "restart your computer. If this screen appears again, follow",
                "these steps:\n",
                "\n",
                "Check to make sure any new hardware or software is properly installed.",
                "If this is a new installation, ask your hardware or software manufacturer",
                "for any Windows updates you might need.\n",
                "\n",
                "If problems continue, disable or remove any newly installed hardware",
                "or software. Disable BIOS memory options such as caching or shadowing.",
                "If you need to access BIOS, restart your computer and enter BIOS setup",
                "by pressing the appropriate key during the startup (usually F2, DEL, or ESC).\n",
                "\n",
                "Technical Information:\n",
                "\n",
                "*** STOP: 0x000000139 (0x0000000000000000, 0xFFFFF80002A12345, 0xFFFFFA8001234000, 0x0000000000000000)",
                "\n",
                "*** ntoskrnl.exe - Address FFFFF80002A12345 base at FFFFF80002A00000, DateStamp 4ce7951a"
            };

            for (int i = 0; i < sizeof(bsodText) / sizeof(bsodText[0]); i++) {
                TextOut(hdc, baseX, baseY, bsodText[i], strlen(bsodText[i]));
                baseY += lineSpacing;
            }

            DeleteObject(hFont);
            EndPaint(hwnd, &ps);
            break;
        }
        
        case WM_CLOSE:
            ShowCursor(TRUE);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "FakeBSODClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        "BSOD",
        WS_POPUP,
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);
    ShowCursor(FALSE);

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
    if (!hKeyboardHook) {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
        return 1;
    }

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);

    return 0;
}
