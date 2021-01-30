/******************************************************************************/
/*!
\file   Graphics_Manager
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\brief
        inits and updates the DX device
*/
/******************************************************************************/


#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <wincodec.h>
#include <windows.h>

#include "Graphics_Manager.h"
#include "Mesh.h"
#include "Sprite.h"
#include "WICTextureLoader.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

D3D_DRIVER_TYPE           g_driverType         = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL         g_featureLevel       = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*             g_pd3dDevice         = nullptr;
ID3D11Device1*            g_pd3dDevice1        = nullptr;
ID3D11DeviceContext*      g_pImmediateContext  = nullptr;
ID3D11DeviceContext1*     g_pImmediateContext1 = nullptr;
IDXGISwapChain*           g_pSwapChain         = nullptr;
IDXGISwapChain1*          g_pSwapChain1        = nullptr;
ID3D11RenderTargetView*   g_pRenderTargetView  = nullptr;
ID3D11VertexShader*       g_pVertexShader      = nullptr;
ID3D11PixelShader*        g_pPixelShader       = nullptr;
ID3D11InputLayout*        g_pVertexLayout      = nullptr;
XMMATRIX                  g_World;
XMMATRIX                  g_View;
XMMATRIX                  g_Projection;
ID3D11Buffer*             g_pConstantBuffer = nullptr;
ID3D11Resource*           g_pTexture        = nullptr;
ID3D11ShaderResourceView* g_pShaderResource = nullptr;

bool    InitTexture();
HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
  HRESULT hr = S_OK;

  RECT rc;
  GetClientRect(GetActiveWindow(), &rc);
  UINT width  = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;

  UINT createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE driverTypes[] = {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    g_driverType = driverTypes[driverTypeIndex];
    hr           = D3D11CreateDevice(nullptr,
                           g_driverType,
                           nullptr,
                           createDeviceFlags,
                           featureLevels,
                           numFeatureLevels,
                           D3D11_SDK_VERSION,
                           &g_pd3dDevice,
                           &g_featureLevel,
                           &g_pImmediateContext);

    if(hr == E_INVALIDARG)
    {
      // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
      hr = D3D11CreateDevice(nullptr,
                             g_driverType,
                             nullptr,
                             createDeviceFlags,
                             &featureLevels[1],
                             numFeatureLevels - 1,
                             D3D11_SDK_VERSION,
                             &g_pd3dDevice,
                             &g_featureLevel,
                             &g_pImmediateContext);
    }

    if(SUCCEEDED(hr))
      break;
  }
  if(FAILED(hr))
    return hr;

  // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
  IDXGIFactory1* dxgiFactory = nullptr;
  {
    IDXGIDevice* dxgiDevice = nullptr;
    hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(SUCCEEDED(hr))
    {
      IDXGIAdapter* adapter = nullptr;
      hr                    = dxgiDevice->GetAdapter(&adapter);
      if(SUCCEEDED(hr))
      {
        hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
        adapter->Release();
      }
      dxgiDevice->Release();
    }
  }
  if(FAILED(hr))
    return hr;

  // Create swap chain
  IDXGIFactory2* dxgiFactory2 = nullptr;
  hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
  if(dxgiFactory2)
  {
    // DirectX 11.1 or later
    hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
    if(SUCCEEDED(hr))
    {
      (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
                                                reinterpret_cast<void**>(&g_pImmediateContext1));
    }

    DXGI_SWAP_CHAIN_DESC1 sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Width              = width;
    sd.Height             = height;
    sd.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount        = 1;

    hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, GetActiveWindow(), &sd, nullptr, nullptr, &g_pSwapChain1);
    if(SUCCEEDED(hr))
    {
      hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
    }

    dxgiFactory2->Release();
  }
  else
  {
    // DirectX 11.0 systems
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                        = 1;
    sd.BufferDesc.Width                   = width;
    sd.BufferDesc.Height                  = height;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = GetActiveWindow();
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;

    hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
  }

  // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
  dxgiFactory->MakeWindowAssociation(GetActiveWindow(), DXGI_MWA_NO_ALT_ENTER);

  dxgiFactory->Release();

  if(FAILED(hr))
    return hr;

  // Create a render target view
  ID3D11Texture2D* pBackBuffer = nullptr;
  hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
  if(FAILED(hr))
    return hr;

  hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
  pBackBuffer->Release();
  if(FAILED(hr))
    return hr;

  g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

  // Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width    = (FLOAT)width;
  vp.Height   = (FLOAT)height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  g_pImmediateContext->RSSetViewports(1, &vp);

  // Compile the vertex shader
  ID3DBlob* pVSBlob = nullptr;
  hr                = CompileShaderFromFile(L"C:/Users/Cody/Perforce/cody.morgan_Wanderer_3106/Project Valiance/Heroic Engine/lib/Shaders/BasicShader.fx", "VS", "vs_4_0", &pVSBlob);
  if(FAILED(hr))
  {
    MessageBox(
        nullptr,
        L"The FXVS file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
        L"Error",
        MB_OK);
    return hr;
  }

  // Create the vertex shader
  hr = g_pd3dDevice->CreateVertexShader(
      pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
  if(FAILED(hr))
  {
    pVSBlob->Release();
    return hr;
  }

  // Define the input layout
  D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  UINT numElements = ARRAYSIZE(layout);

  // Create the input layout
  hr = g_pd3dDevice->CreateInputLayout(
      layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
  pVSBlob->Release();
  if(FAILED(hr))
    return hr;

  // Set the input layout
  g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

  // Compile the pixel shader
  ID3DBlob* pPSBlob = nullptr;
  hr                = CompileShaderFromFile(L"../Heroic Engine/lib/Shaders/TextShader.fx", "PS", "ps_4_0", &pPSBlob);
  if(FAILED(hr))
  {
    MessageBox(
        nullptr,
        L"The FXPS file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
        L"Error",
        MB_OK);
    return hr;
  }

  // Create the pixel shader
  hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
  pPSBlob->Release();
  if(FAILED(hr))
    return hr;

  // Create vertex buffer

  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  // Create the constant buffer
  // 2 parts
  //
  bd.Usage          = D3D11_USAGE_DEFAULT;
  bd.ByteWidth      = sizeof(ConstantBuffer);
  bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  hr                = GetDevicePointer()->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
  if(FAILED(hr))
    return hr;

  // Initialize the world matrix
  g_World = XMMatrixIdentity();

  // Initialize the view matrix
  XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
  XMVECTOR At  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMVECTOR Up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  g_View       = XMMatrixLookAtLH(Eye, At, Up);

  // Initialize the projection matrix
  g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (float)height, 0.01f, 100.0f);

  bool InitTexture();
  return S_OK;
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
  if(g_pImmediateContext)
    g_pImmediateContext->ClearState();

  // if (g_pVertexBuffer) g_pVertexBuffer->Release();
  if(g_pVertexLayout)
    g_pVertexLayout->Release();
  if(g_pVertexShader)
    g_pVertexShader->Release();
  if(g_pPixelShader)
    g_pPixelShader->Release();
  if(g_pRenderTargetView)
    g_pRenderTargetView->Release();
  if(g_pSwapChain1)
    g_pSwapChain1->Release();
  if(g_pSwapChain)
    g_pSwapChain->Release();
  if(g_pImmediateContext1)
    g_pImmediateContext1->Release();
  if(g_pImmediateContext)
    g_pImmediateContext->Release();
  if(g_pd3dDevice1)
    g_pd3dDevice1->Release();
  if(g_pd3dDevice)
    g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
  HRESULT hr = S_OK;

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows
  // the shaders to be optimized and to run exactly the way they will run in
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;

  // Disable optimizations to further improve shader debugging
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ID3DBlob* pErrorBlob = nullptr;
  hr                   = D3DCompileFromFile(
      szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
  if(FAILED(hr))
  {
    if(pErrorBlob)
    {
      OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
      pErrorBlob->Release();
    }
    return hr;
  }
  if(pErrorBlob)
    pErrorBlob->Release();

  return S_OK;
}

void UpdateConstantBuffer(ConstantBuffer* localConstantBuffer)
{
  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, localConstantBuffer, 0, 0);
  g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
}
//--------------------------------------------------------------------------------------
// Render this frame
//--------------------------------------------------------------------------------------
void Render()
{
  // clear the screen to this color
  // TODO: dirty flags

  // Update variables
  g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
  g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

  // Present our back buffer to our front buffer
  g_pSwapChain->Present(0, 0);
  g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);
}

//--------------------------------------------------------------------------------------
// Getters
//--------------------------------------------------------------------------------------
ID3D11Device* GetDevicePointer()
{
  return g_pd3dDevice;
}

ID3D11DeviceContext* GetDeviceContext()
{
  return g_pImmediateContext;
}

bool InitTexture()
{
  HRESULT hr = DirectX::CreateWICTextureFromFile(
      g_pd3dDevice,
      L"C:/Users/Cody/Perforce/cody.morgan_Wanderer_3106/Project Valiance/Heroic Engine/Assets/badass.png",
      &g_pTexture,
      &g_pShaderResource);
  if(FAILED(hr))
  {
    printf("invalid texture%c\n", hr);
    return false;
  }
  return true;
}
