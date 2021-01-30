/******************************************************************************/
/*!
\file   Sprite
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\brief
        Manages sprite components
        will contain animations as well
*/
/******************************************************************************/

#include "Component.h"
#include "Entity.h"
#include "Graphics_Manager.h"
#include "Mesh.h"
#include "Sprite.h"
#include "Sprite_Manager.h"
#include "Transform.h"
#include "DirectXMath.h"
extern ID3D11ShaderResourceView* g_pShaderResource;

Sprite::Sprite() : mesh_(new Mesh()), thisCB_(new ConstantBuffer)
{
  transform_ = XMMatrixIdentity();

  // register this sprite with the manager
  Sprite_Manager* spm = Sprite_Manager::GetSpriteManager();
  spm->Add(this);
}

Sprite::~Sprite()
{
  delete mesh_;
  mesh_ = nullptr;
}

void Sprite::Draw()
{
	transform_ = parent->GetComponent<Transform>()->matrix;
	Transform* transform = parent->GetComponent<Transform>();
  // Initialize the view matrix
  // TODO: move to camera systemS
  XMVECTOR Eye    = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
  XMVECTOR At     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMVECTOR Up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMMATRIX g_View = XMMatrixLookAtLH(Eye, At, Up);

  XMVECTOR zero = { 0,0,0 };


  // Initialize the projection matrix
  RECT rc;
  GetClientRect(GetActiveWindow(), &rc);
  UINT     width        = rc.right - rc.left;
  UINT     height       = rc.bottom - rc.top;
  XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (float)height, 0.01f, 100.0f);

 transform_ = DirectX::XMMatrixTransformation2D(zero,
	  1,
	 transform->scale,
	 zero,
	 transform->rotation,
	 transform->translation);
  thisCB_->mWorld      = XMMatrixTranspose(transform_);
  thisCB_->mView       = XMMatrixTranspose(g_View);
  thisCB_->mProjection = XMMatrixTranspose(g_Projection);

  // Set vertex buffer
  // TODO move to mesh render
  UINT          stride          = sizeof(Vertex);
  UINT          offset          = 0;
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
  int                  count               = mesh_->getIndexCount();
  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pShaderResource);
  g_pImmediateContext->DrawIndexed(count, 0, 0);  // 36 vertices needed for 12 triangles in a triangle list

  UpdateConstantBuffer(thisCB_);
}

bool Sprite::operator>(const Sprite& sprite)
{
  return (this->tag_ > sprite.tag_);
}

bool GreaterThan(const Sprite* lhs, const Sprite* rhs)
{
  return ((*lhs).getTag() > (*rhs).getTag());
}

int Sprite::getTag() const
{
  return tag_;
}

bool Sprite::isDestroyed()
{
  return isDestroyed_;
}

void Sprite::setTag(spriteTag tag)
{
  tag_ = tag;
}

void Sprite::setDestroy(bool isDestroyed)
{
  isDestroyed_ = isDestroyed;
}
