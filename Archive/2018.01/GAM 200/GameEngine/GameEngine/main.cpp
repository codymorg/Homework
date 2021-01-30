#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "Global.h"
#include "Graphics_Manager.h"
#include "resource.h"
#include "Sprite.h"
#include "Mesh.h"

using namespace DirectX;


HINSTANCE g_hInst = nullptr;
HWND      g_hWnd = nullptr;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance,// A handle to this instance. This is the base address of the module in memory.
  _In_opt_ HINSTANCE hPrevInstance,          // A handle to the previous instance
  _In_ LPWSTR lpCmdLine,                     // The command line for the application, excluding the program name.
  _In_ int nCmdShow)                         // Controls how the window is to be shown //SW_MAXIMIZE
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  if (FAILED(InitWindow(hInstance, nCmdShow)))
    return 0;

  if (FAILED(InitDevice()))
  {
    CleanupDevice();
    return 0;
  }

  // Main message loop
  MSG msg = { 0 };
  Sprite sp;
  Sprite sp2;
  sp2.transform_ = XMMatrixTranslation(2, 2, 0);

  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      Render(&sp);
      Render(&sp2);

    }
  }

  CleanupDevice();

  return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);                                 // size of this structure
  wcex.style = CS_HREDRAW | CS_VREDRAW;                             // class style
  wcex.lpfnWndProc = WndProc;                                       // *window procedure
  wcex.cbClsExtra = 0;                                              // # trailing bytes
  wcex.cbWndExtra = 0;                                              // # trailing bytes to instance
  wcex.hInstance = hInstance;                                       // this instance
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);         // class icon
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);                    // class cursor
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                  // class background
  wcex.lpszMenuName = nullptr;                                      // resource name of the class menu
  wcex.lpszClassName = L"TutorialWindowClass";                       // window class name
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);  // window icon

  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  // Create window
  g_hInst = hInstance;
  RECT rc = { 0, 0, 800, 600 };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 3: Shaders",
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
    nullptr);
  if (!g_hWnd)
    return E_FAIL;

  ShowWindow(g_hWnd, nCmdShow);

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

    // Note that this tutorial does not handle resizing (WM_SIZE) requests,
    // so we created the window without the resize border.

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}


HINSTANCE Get_hInst()
{
  return g_hInst;
}
HWND Get_hWnd()
{
  return g_hWnd;
}
