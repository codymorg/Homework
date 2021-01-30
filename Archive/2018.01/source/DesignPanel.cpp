/******************************************************************************/
/*!
\file   DesignPanel.cpp
\author Werner Chen
\par    email: werner.chen\@digipen.edu

\date   09/18/2018
\brief
 Imgui class DesignPanel implementation. There should be only one iteration of 
 it declared and initialized at the initializing phase of the engine

*/
/******************************************************************************/
#include <iostream>
#include "DesignPanel.h"
#include <algorithm>
//#include "Panels.h"



DesignPanel::DesignPanel()
{
	
}

void DesignPanel::RegistPanels(void (*f)(std::map<std::string, void*>)){
	m_panelsFunctions.push_back(f);
}
void DesignPanel::BindDirectX(ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pDeviceContext, IDXGISwapChain* g_pSwapChain)
{
	mp_pd3dDevice = g_pd3dDevice;
	mp_pDeviceContext = g_pDeviceContext;
	mp_pSwapChain = g_pSwapChain;
}
bool DesignPanel::Initialze() {
	if (!CreateDxWindow((char*)mp_windowName, DESIGN_WINDOS_POSX, DESIGN_WINDOS_POSY, DESIGN_WINDOW_WIDTH, DESIGN_WINDOW_HEIGHT))
		return false;
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(mp_pd3dDevice, mp_pDeviceContext);

	ImGui::StyleColorsDark();

	for (const auto & f:m_panelsFunctions){ 
		f(m_variables);
	}
	return true;
}

void DesignPanel::Run() {
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

DesignPanel::~DesignPanel()
{
	UnregisterClass(mp_windowName, m_hInst);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool DesignPanel::CreateDxWindow(char* windowTitle, int x, int y, int width, int height) {
	WNDCLASSEX wcex;
	
	m_hInst = GetModuleHandle(NULL);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;                                       // *window procedure
	wcex.cbClsExtra = 0;                                              // # trailing bytes
	wcex.cbWndExtra = 0;                                              // # trailing bytes to instance
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);                    // class cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                  // class background
	wcex.lpszMenuName = nullptr;                                      // resource name of the class menu
	wcex.lpszClassName = DESIGN_PANEL_NAME;
	wcex.cbSize = sizeof(WNDCLASSEX);

	if(!RegisterClassEx(&wcex)){
		MessageBox(NULL, L"RegiserClassEx() failed", L"Error", MB_OK);
		return false;

	}

	
	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, DESIGN_PANEL_NAME, DESIGN_PANEL_NAME,nStyle,x,y,width,height,NULL,NULL,m_hInst,NULL);
	
	if (m_hWnd == NULL) {
		MessageBox(NULL, L"CreateWindowEx() failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd); 
	return true;
}
LRESULT CALLBACK DesignPanel::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		DestroyWindow(hWnd);
	}break;
		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void DesignPanel::Bind(std::string name, void* v_pointer) {
	m_variables[name] = v_pointer;
}