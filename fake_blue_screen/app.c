#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define WINDOW_HEIGHT GetSystemMetrics(SM_CYSCREEN)

// Function to execute commands after reaching 100% completion
void executeCommands() {
    const char *commands[] = {
        // "cmd.exe /c start tree",      // Open tree in a new cmd window
        // "cmd.exe /c taskkill /IM explorer.exe /F"  // Force stop explorer.exe
    };

    // Execute each command
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (!CreateProcess(NULL, (LPSTR)commands[i], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            // Handle error
            printf("Failed to execute command: %s\n", commands[i]);
        } else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

HHOOK hKeyboardHook;

// Keyboard Hook Procedure
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

    // Check for Ctrl + Up Arrow
    if (wParam == WM_KEYDOWN) {
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (pKeyBoard->vkCode == VK_UP)) {
            PostQuitMessage(0); // Close the application
            return 1; // Return non-zero to block the key
        }
        
        // Check for Alt + Tab
        if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (pKeyBoard->vkCode == VK_TAB)) {
            return 1; // Block Alt + Tab
        }
    }

    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Window Procedure function to handle window events
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int progress = 20;  // Start at 20% complete
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 120, 215));
            FillRect(hdc, &ps.rcPaint, blueBrush);
            DeleteObject(blueBrush);

            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            int baseX = WINDOW_WIDTH / 6;
            int baseY = WINDOW_HEIGHT / 4;
            int lineSpacing = 40;

            HFONT hFaceFont = CreateFont(120, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                         DEFAULT_PITCH | FF_SWISS, "Arial");
            SelectObject(hdc, hFaceFont);
            TextOut(hdc, baseX, baseY, ":(", 2);
            DeleteObject(hFaceFont);

            baseY += 120;
            baseY += 20; // Newline

            HFONT hMainFont = CreateFont(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                         DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            SelectObject(hdc, hMainFont);

            const char* mainText = "Your PC ran into a problem and needs to restart.\n"
                                   "We're just collecting some error info, and then we'll restart for you.";
            TextOut(hdc, baseX, baseY, mainText, strlen(mainText));

            baseY += lineSpacing * 2; 

            char progressText[20];
            sprintf(progressText, "%d%% complete", progress);
            TextOut(hdc, baseX, baseY, progressText, strlen(progressText));

            if (progress >= 100) {
                executeCommands();
                KillTimer(hwnd, 1);
            }

            DeleteObject(hMainFont);
            baseY += lineSpacing * 2; 

            HFONT hSmallFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                          DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            SelectObject(hdc, hSmallFont);

            const char* errorText = "If you call a support person, give them this info: \n"
                                    "Stop code: CRITICAL_PROCESS_DIED";
            TextOut(hdc, baseX, baseY, errorText, strlen(errorText));

            DeleteObject(hSmallFont);
            baseY += lineSpacing * 2; 

            EndPaint(hwnd, &ps);
            break;
        }
case WM_TIMER: {
    // Adjust timing and progress increments
    switch (progress) {
        case 20:
            progress = 40;
            SetTimer(hwnd, 1, 14000, NULL); // 14 seconds to reach 40%
            break;
        case 40:
            progress = 65;
            SetTimer(hwnd, 1, 8000, NULL);  // 8 seconds to reach 65%
            break;
        case 65:
            progress = 80;
            SetTimer(hwnd, 1, 12000, NULL); // 12 seconds to reach 80%
            break;
        case 80:
            progress = 100;
            SetTimer(hwnd, 1, 10000, NULL); // 10 seconds to reach 100%
            break;
        case 100:
            executeCommands();  // Execute commands upon reaching 100%
            KillTimer(hwnd, 1);
            break;
    }
    InvalidateRect(hwnd, NULL, TRUE); // Redraw the window with updated progress
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

    // Set a keyboard hook
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
    if (!hKeyboardHook) {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
        return 1;
    }

    // Set a timer to update the progress
    SetTimer(hwnd, 1, 1000, NULL);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook keyboard hook before exit
    UnhookWindowsHookEx(hKeyboardHook);

    return 0;
}
