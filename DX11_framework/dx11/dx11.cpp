// include the basic windows header file
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#include <iostream>
using std::cout;
#include <string>
using std::string;
using std::wstring;

#include "Graphics.h"
#include "Vertex.h"

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void openConsole()
{
  AllocConsole();
  FILE *fDummy;
  freopen_s(&fDummy, "CONIN$", "r", stdin);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
  cout << "console window allocated\n";
}

HWND openWindow(HINSTANCE hInstance, string windowName, int nCmdShow, unsigned width=0, unsigned height=0)
{
  // the handle for the window, filled by a function
  HWND hWnd = GetDesktopWindow();

  // this struct holds information for the window class
  WNDCLASSEX wc;

  // clear out the window class for use
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  // fill in the struct with the needed information
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszClassName = L"WindowClass1";

  // register the window class
  RegisterClassEx(&wc);

  // create the window and use the result as the handle
  wstring wide(windowName.begin(), windowName.end());
  if (width == 0 || height == 0)
  {
    RECT rect;
    GetWindowRect(hWnd, &rect);
    width = rect.right;
    height = rect.bottom;
  }

  hWnd = CreateWindowEx(NULL,
                        wc.lpszClassName,    // name of the window class
                        wide.c_str(),        // title of the window
                        WS_OVERLAPPEDWINDOW, // window style
                        0,                   // x-position of the window
                        0,                   // y-position of the window
                        width,               // width of the window
                        height,              // height of the window
                        NULL,                // we have no parent window, NULL
                        NULL,                // we aren't using menus, NULL
                        hInstance,           // application handle
                        NULL);               // used with multiple windows, NULL

  // display the window on the screen
  ShowWindow(hWnd, nCmdShow);

  return hWnd;
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  openConsole();
  auto hWnd = openWindow(hInstance, "DirectX 11 framework", nCmdShow);

  Engine engine(hWnd);
  engine.addVertexShader("vert.vs");
  engine.addPixelShader("pix.ps");
  engine.setActiveShaders();
  std::vector<Vertex> verts = 
  {
    {0.0f, 0.5f, 0.0f, Color(1.0f, 0.0f, 0.0f, 1.0f)},
    {0.45f, -0.5, 0.0f, Color(0.0f, 1.0f, 0.0f, 1.0f)},
    {-0.45f, -0.5f, 0.0f, Color(0.0f, 0.0f, 1.0f, 1.0f)}
  };

  engine.mapVerts(verts);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    // translate keystroke messages into the right format
    TranslateMessage(&msg);

    // send the message to the WindowProc function
    DispatchMessage(&msg);

    engine.render();
  }

  // return this part of the WM_QUIT message to Windows
  return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // sort through and find what code to run for the message given
  switch (message)
  {
    case WM_DESTROY:
      // close the application entirely
      PostQuitMessage(0);
      return 0;
    case WM_SIZE:
      cout << "resizing window\n";
  }

  // Handle any messages the switch statement didn't
  return DefWindowProc(hWnd, message, wParam, lParam);
}
