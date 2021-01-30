#include <directxmath.h>
#include <d3d11.h>
#include <iostream>


#include "Mesh.h"
#include "Graphics_Manager.h"
#include <vector>


/******************************************************************************
*   Vertex Functions
******************************************************************************/

  Vertex::Vertex(XMFLOAT3 position) :
    position_(position), color_(1, 0, 1, 1){}

  Vertex::Vertex(XMFLOAT3 position, XMFLOAT4 color):
    position_(position), color_(color){}

  Vertex& Vertex::operator+=(const Vertex& rhs)
  {
    this->position_.x += rhs.position_.x;
    this->position_.y += rhs.position_.y;
    this->position_.z += rhs.position_.z;

    return *this;
  }

/******************************************************************************
*   Mesh Functions
******************************************************************************/

  Mesh::Mesh(std::vector<Vertex> vertexList, std::vector<WORD> indexList):
    vertexList_(vertexList), indexList_(indexList)
  {}

  //makes a default unit sqaure and closes the mesh
  Mesh::Mesh()
  {
    Vertex verts[] =
    {
      { XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },//0
      { XMFLOAT3( 0.5f, 0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },//1
      { XMFLOAT3( 0.5f,-0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },//2
      { XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },//0
      { XMFLOAT3( 0.5f,-0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },//2
      { XMFLOAT3(-0.5f,-0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },//3
    };
    addTriangleLH(verts[0], verts[1], verts[2]);
    addTriangleLH(verts[3], verts[4], verts[5]);
    closeMesh();

  }

  void Mesh::closeMesh(D3D_PRIMITIVE_TOPOLOGY topology)
  {
    /////////////////////////////////////////////TEMP
    struct ConstantBuffer
    {
      XMMATRIX mWorld;
      XMMATRIX mView;
      XMMATRIX mProjection;
    };
    /////////////////////////////////////////////TEMP

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * int(vertexList_.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &(vertexList_[0]); // pointer to the actual vertex info

    HRESULT hr = GetDevicePointer()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
    if (FAILED(hr))
      throw("vertex buffer creation failed");

    // Create index buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * int(indexList_.size());        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = &(indexList_[0]); // pointer to actual index list
    hr = GetDevicePointer()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
    if (FAILED(hr))
      throw("index buffer creation failed");

    // Set index buffer
    GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    GetDeviceContext()->IASetPrimitiveTopology(topology);
  }

  Mesh::~Mesh()
  {
    vertexList_.~vector();
    indexList_.~vector();
    g_pVertexBuffer = nullptr;
    g_pIndexBuffer = nullptr;
    //g_pConstantBuffer = nullptr;
  }

  void Mesh::addTriangleLH(Vertex a, Vertex b, Vertex c)
  {
    //update the vertex list so others dont have to do memory control of meshes
    addVertex(a);
    addVertex(b);
    addVertex(c);

  }

  void Mesh::addVertex(Vertex a)
  {
    vertexList_.push_back(a);
    indexList_.push_back(WORD(indexList_.size()));
  }

  Vertex* Mesh::getVertexList()
  {
    return &(vertexList_[0]);
  }

  WORD* Mesh::getIndexList()
  {
    return &(indexList_[0]);
  }

  ID3D11Buffer* Mesh::getVertexBuffer()
  {
    return g_pVertexBuffer;
  }

  ID3D11Buffer* Mesh::getIndexBuffer()
  {
    return g_pIndexBuffer;
  }

  void Mesh::setTopology(D3D_PRIMITIVE_TOPOLOGY topology)
  {
    topology_ = topology;
  }

  D3D_PRIMITIVE_TOPOLOGY Mesh::getTopology()
  {
    return topology_;

  }

  unsigned Mesh::getIndexCount()
  {
    return unsigned(indexList_.size());
  }

