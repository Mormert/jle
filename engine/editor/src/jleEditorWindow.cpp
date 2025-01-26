/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleEditorWindow.h"

#include <GLFW/glfw3.h>

#ifdef WIN32
#include <ImGui/ImGui.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windef.h>
#include <windowsx.h>
#endif

namespace
{

#ifdef WIN32

WNDPROC g_glfwWndProc = nullptr;

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    constexpr int customTitleBarHeight = 30;
    static bool resizing = false;
    static POINT lastCursorPosition;

    RECT windowRect{};

    switch (uMsg) {
    case WM_NCCALCSIZE:
        // Remove standard window frame
        return 0;
    case WM_NCHITTEST:
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(hWnd, &cursor);

        if (ImGui::IsAnyItemHovered()) {
            return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
        }

        GetClientRect(hWnd, &windowRect);

        // Adjust the bottom border for resizing
        windowRect.bottom += 5;

        if (cursor.y >= windowRect.bottom - 5) {
            if (cursor.x < 5)
                return HTBOTTOMLEFT;
            else if (cursor.x > windowRect.right - 5)
                return HTBOTTOMRIGHT;
            else
                return HTBOTTOM;
        } else if (cursor.x < 5) {
            if (cursor.y < 5)
                return HTTOPLEFT;
            else
                return HTLEFT;
        } else if (cursor.x > windowRect.right - 5) {
            if (cursor.y < 5)
                return HTTOPRIGHT;
            else
                return HTRIGHT;
        } else if (cursor.y < customTitleBarHeight) {
            if (cursor.y < 5) {
                if (cursor.x < windowRect.right / 2)
                    return HTTOPLEFT;
                else
                    return HTTOPRIGHT;
            } else
                return HTCAPTION;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam); // Return default handling for other areas
    case WM_LBUTTONDOWN:
        if (wParam == HTTOPLEFT || wParam == HTTOPRIGHT || wParam == HTBOTTOMLEFT || wParam == HTBOTTOMRIGHT ||
            wParam == HTBOTTOM || wParam == HTLEFT || wParam == HTRIGHT) {
            resizing = true;
            lastCursorPosition = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetCapture(hWnd);
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_MOUSEMOVE:
        if (resizing) {
            POINT currentCursorPosition = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int deltaX = currentCursorPosition.x - lastCursorPosition.x;
            int deltaY = currentCursorPosition.y - lastCursorPosition.y;

            if (wParam == HTTOPLEFT || wParam == HTLEFT || wParam == HTBOTTOMLEFT) {
                windowRect.left += deltaX;
                windowRect.top += deltaY;
            }
            if (wParam == HTTOPLEFT || wParam == HTTOP || wParam == HTTOPRIGHT) {
                windowRect.top += deltaY;
            }
            if (wParam == HTTOPLEFT || wParam == HTTOPRIGHT || wParam == HTBOTTOMRIGHT) {
                windowRect.right += deltaX;
            }
            if (wParam == HTTOPRIGHT || wParam == HTRIGHT || wParam == HTBOTTOMRIGHT) {
                windowRect.bottom += deltaY;
            }

            SetWindowPos(hWnd,
                         NULL,
                         windowRect.left,
                         windowRect.top,
                         windowRect.right - windowRect.left,
                         windowRect.bottom - windowRect.top,
                         SWP_NOZORDER | SWP_NOREDRAW);
            lastCursorPosition = currentCursorPosition;
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_LBUTTONUP:
        if (resizing) {
            resizing = false;
            ReleaseCapture();
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_GETMINMAXINFO: {
        // Set maximum and minimum window size and position based on screen dimensions
        auto *pmmi = (MINMAXINFO *)lParam;

        pmmi->ptMaxPosition.x = 0;
        pmmi->ptMaxPosition.y = 0;

        // Get the monitor where the window is located
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        if (hMonitor) {
            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfo(hMonitor, &monitorInfo)) {
                RECT workArea = monitorInfo.rcWork;
                int workAreaWidth = workArea.right - workArea.left;
                int workAreaHeight = workArea.bottom - workArea.top;

                pmmi->ptMaxSize.x = workAreaWidth;
                pmmi->ptMaxSize.y = workAreaHeight;
            }
        }

        return 0;
    }
    default:
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    }
}

static void
initEditorWindowWin32(GLFWwindow* glfwWindow,  jleWindow &window)
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    RECT workArea;
    int workAreaWidth = mode->width;
    int workAreaHeight = mode->height;

    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0)) {
        workAreaWidth = ((float)workArea.right - (float)workArea.left);
        workAreaHeight = ((float)workArea.bottom - (float)workArea.top);
    }

    // Get the native window handle
    HWND hwnd = glfwGetWin32Window(glfwWindow);

    // Saves a reference to the WndProc function set from GLFW (found in glfw's win32_window.c)
    g_glfwWndProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);

    // Set the window procedure callback
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);

    // WE_EX_TOOLWINDOW is used to combat an issue where the window borders would be "outside the screen"
    // WS_EX_APPWINDOW is to combat an issue which is caused by WE_EX_TOOLWINDOW, which is that it disables the
    // program from showing up in the Windows taskbar
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);

    // Maximises the window on startup
    ShowWindow(hwnd, SW_MAXIMIZE);
}

#endif

} // namespace

GLFWwindow *
jleEditorWindow::initGlfwWindow(int width, int height, const char *title)
{
    GLFWwindow* glfwWindow;
#ifdef WIN32
    glfwWindow = jleWindow::initGlfwWindow(width, height, title);
    initEditorWindowWin32(glfwWindow, *this);
#else
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindow = jleWindow::initGlfwWindow(mode->width, mode->height, title);
#ifdef __APPLE__
    customizeTitleBarMacOS(glfwWindow);
#endif
#endif
    return glfwWindow;
}
