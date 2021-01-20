#include <d3dcompiler.h>
#include <windows.h>
#include <windowsx.h>

#include <iostream>
using std::cout;
#include <string>
using std::string;
using std::wstring;
#include "Graphics.h"

// include the Direct3D Library file
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

Engine::Engine(HWND hWnd)
{

  init(hWnd);
  setRenderTarget();
  RECT rect;
  GetWindowRect(hWnd, &rect);
  setViewport(rect.right, rect.bottom);
  generateVertexBuffer();
}

Engine::~Engine()
{
  // close and release all existing COM objects
  pVS_->Release();
  pPS_->Release();
  backbuffer_->Release();
  swapchain_->Release();
  devcon_->Release();
  device_->Release();
}

void Engine::render()
{
  // clear the back buffer to a deep blue
  const float color[] = {0.0f, 0.2f, 0.4f, 1.0f};
  devcon_->ClearRenderTargetView(backbuffer_, color);

  // do 3D rendering on the back buffer here
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  devcon_->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
  // select which primtive type we are using
  devcon_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  devcon_->Draw(3, 0);

  // switch the back buffer and the front buffer
  if (auto res = swapchain_->Present(0, 0))
    error(res);


}

unsigned Engine::addVertexShader(std::string location)
{
  compileShader(location, "vs");
  return 0;
}

unsigned Engine::addPixelShader(std::string location)
{
  compileShader(location, "ps");
  return 0;
}

void Engine::error(HRESULT error)
{
  cout << "failed to initialize graphics engine HRESULT: " << error;
  throw "failed to initialalize engine";
}

void Engine::init(HWND hWnd)
{
  // create a struct to hold information about the swap chain
  DXGI_SWAP_CHAIN_DESC scd;

  // clear out the struct for use
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  // fill the swap chain description struct
  scd.BufferCount       = 1;                               // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // use 32-bit color
  scd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
  scd.OutputWindow      = hWnd;                            // the window to be used
  scd.SampleDesc.Count  = 4;                               // how many multisamples
  scd.Windowed          = TRUE;                            // windowed/full-screen mode

  // create a device, device context and swap chain using the information in the scd struct
  if (auto res = D3D11CreateDeviceAndSwapChain(NULL,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               D3D11_SDK_VERSION,
                                               &scd,
                                               &swapchain_,
                                               &device_,
                                               NULL,
                                               &devcon_))
  {
    error(res);
  }

  cout << "DirectX 11 initialized\n";
}

void Engine::setRenderTarget()
{
  // get the address of the back buffer
  ID3D11Texture2D* pBackBuffer;
  if (auto res = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))
    error(res);

  // use the back buffer address to create the render target
  if (auto res = device_->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer_))
    error(res);

  pBackBuffer->Release();

  // set the render target as the back buffer
  devcon_->OMSetRenderTargets(1, &backbuffer_, NULL);

  cout << "Render Target set\n";
}

void Engine::setViewport(unsigned viewWidth, unsigned viewHeight)
{
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width    = viewWidth;
  viewport.Height   = viewHeight;

  devcon_->RSSetViewports(1, &viewport);

  cout << "viewport set to " << viewport.Width << " X " << viewport.Height << '\n';
}

void Engine::compileShader(std::string location, std::string shaderType)
{
  wstring loc(location.begin(), location.end());
  shaderType += "_4_0";
  ID3D10Blob* shaderBlob;



  HRESULT res = S_FALSE;
  if (shaderType.compare("vs_4_0") == 0)
  {
    if (auto res = D3DCompileFromFile(loc.c_str(), 0, 0, "main", shaderType.c_str(), 0, 0, &shaderBlob, 0))
      error(res);
    res = device_->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pVS_);
    D3D11_INPUT_ELEMENT_DESC ied[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    if (res = device_->CreateInputLayout(
          ied, _countof(ied), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pLayout))
    {
      error(res);
    }
    devcon_->IASetInputLayout(pLayout);
  }
  else
  {
    if (auto res = D3DCompileFromFile(loc.c_str(), 0, 0, "main", shaderType.c_str(), 0, 0, &shaderBlob, 0))
      error(res);
    res = device_->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pPS_);
  }
  if (res)
    error(res);


  printf("Compiled %s shader from %s\n", shaderType.c_str(), location.c_str());
}

// with multiple shaders this wil need to change
void Engine::setActiveShaders()
{
  devcon_->VSSetShader(pVS_, 0, 0);
  devcon_->PSSetShader(pPS_, 0, 0);
  cout << "VS and PS shaders set to active\n";
}

void Engine::mapVerts(std::vector<Vertex> verts)
{
  D3D11_MAPPED_SUBRESOURCE ms;
  devcon_->Map(vBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // map the buffer
  memcpy(ms.pData, &(verts[0]), sizeof(verts[0].x) * 7 * verts.size());  
  auto y = sizeof(verts[0].x) * 7 * verts.size(); // copy the data
  auto f = sizeof(0.0f);
  devcon_->Unmap(vBuffer, NULL);                                   // unmap the buffer
}

void Engine::generateVertexBuffer()
{
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage          = D3D11_USAGE_DYNAMIC;      // write access access by CPU and GPU
  bd.ByteWidth      = sizeof(Vertex) * 3;       // size is the VERTEX struct * 3
  bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   // allow CPU to write in buffer

  if (auto dev = device_->CreateBuffer(&bd, NULL, &vBuffer))
    error(dev);
}


