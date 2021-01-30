#include "Sprite.h"
#include "Mesh.h"
#include "Global.h"
#include "Graphics_Manager.h"

Sprite::Sprite() : mesh_(new Mesh()), thisCB_(new ConstantBuffer)
{
  transform_ = XMMatrixIdentity();
}

Sprite::~Sprite()
{
  delete mesh_;
  mesh_ = nullptr;
}

void Sprite::draw()
{
  // Initialize the view matrix
  //TODO: move to camera system
  XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
  XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMMATRIX g_View = XMMatrixLookAtLH(Eye, At, Up);

  // Initialize the projection matrix
  RECT rc;
  GetClientRect(Get_hWnd(), &rc);
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;
  XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (float)height, 0.01f, 100.0f);

  thisCB_->mWorld = XMMatrixTranspose(transform_);
  thisCB_->mView = XMMatrixTranspose(g_View);
  thisCB_->mProjection = XMMatrixTranspose(g_Projection);

  //Set vertex buffer
  //TODO move to mesh render
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  ID3D11Buffer* g_pVertexBuffer = (mesh_->getVertexBuffer());
  GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

  // Set index buffer
  ID3D11Buffer* g_pIndexBuffer = (mesh_->getIndexBuffer());
  GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

  // Set primitive topology
  if(mesh_->getTopology() != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
    GetDeviceContext()->IASetPrimitiveTopology(mesh_->getTopology());

  // Renders a triangle
  ID3D11DeviceContext* g_pImmediateContext = GetDeviceContext();
  int count = mesh_->getIndexCount();
  g_pImmediateContext->DrawIndexed(count, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

  UpdateConstantBuffer(thisCB_);
}

