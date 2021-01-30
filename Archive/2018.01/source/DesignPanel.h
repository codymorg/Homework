#pragma once
#ifndef  DESIGNPANEL_H
#define  DESIGNPANEL_H


#include "../../Platform/source/Imgui\imgui.h"
#include "../../Platform/source/Imgui\imgui_impl_win32.h"
#include "../../Platform/source/Imgui\imgui_impl_dx11.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <map>
#include <vector>

#define DESIGN_PANEL_NAME L"Design Tool"
#define DESIGN_WINDOW_WIDTH 1200
#define DESIGN_WINDOW_HEIGHT 760
#define DESIGN_WINDOS_POSX 50
#define DESIGN_WINDOS_POSY 50

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class DesignPanel
{
public:
	DesignPanel();
	~DesignPanel();
	
	bool Initialze();
	void BindDirectX(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*);
	void Bind(std::string name,void*);
	void RegistPanels(void (*f)(std::map<std::string, void*>));
	void Run();

private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool CreateDxWindow(char* windowTitle, int x, int y, int width, int height);
	LPCWSTR   mp_windowName = DESIGN_PANEL_NAME;
	HINSTANCE m_hInst = nullptr;
	HWND      m_hWnd =  nullptr;
	ID3D11Device* mp_pd3dDevice = nullptr;
	ID3D11DeviceContext*    mp_pDeviceContext = nullptr;
	IDXGISwapChain*         mp_pSwapChain = nullptr;
	ID3D11RenderTargetView* mp_pRenderTargetView = nullptr;
	std::map<std::string, void*> m_variables;
	std::vector<void (*)(std::map<std::string,void*>)> m_panelsFunctions;

};

#endif // ! DESIGNPANEL_H