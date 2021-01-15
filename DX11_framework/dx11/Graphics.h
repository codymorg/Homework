#pragma once
#include <d3d11.h>
#include <windef.h>

#include <map>
#include <string>
#include <vector>

class Engine
{
public:
  Engine(HWND hWnd);
  ~Engine();

  void     render();
  unsigned addVertexShader(std::string location);
  unsigned addPixelShader(std::string location);

private:
  IDXGISwapChain*         swapchain_;  // the pointer to the swap chain interface
  ID3D11Device*           device_;     // the pointer to our Direct3D device interface
  ID3D11DeviceContext*    devcon_;     // the pointer to our Direct3D device context
  ID3D11RenderTargetView* backbuffer_; // render target (swapchain)

  std::vector<std::string> vertexShaders_;
  std::vector<std::string> pixelShaders_;
  ID3D11VertexShader*      pVS_;
  ID3D11PixelShader*       pPS_;

  void error(HRESULT error);
  void init(HWND hWnd);
  void setRenderTarget();
  void setViewport(unsigned viewWidth, unsigned viewHeight);
  void compileShader(std::string location, std::string shaderType);
  void setActiveShaders();
};
