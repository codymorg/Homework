#pragma once
#ifndef Mesh_h
#define Mesh_h

#include <directxmath.h>
#include <d3d11.h>
#include <vector>

using namespace DirectX;

const XMFLOAT4 defaultColor(1, 0, 1, 1);

/******************************************************************************
* Contains a single vertex
******************************************************************************/
class Vertex
{
public:
  Vertex(XMFLOAT3 position);
  Vertex(XMFLOAT3 position, XMFLOAT4 color);

  Vertex& operator+=(const Vertex& rhs);

private:
  XMFLOAT3 position_;
  XMFLOAT4 color_;
};


/******************************************************************************
* Contains a series of vertices forming a mesh
******************************************************************************/
class Mesh
{
  public:
  //make a custom mesh YOU MUST CLOSE THE MESH
  Mesh(std::vector<Vertex> vertexList, std::vector<WORD> indexList);

  //make a default unit square you do not have to close the mesh
  Mesh();
  ~Mesh();

  // ***** Getters ***** //
  Vertex*                getVertexList();     //get the actual list of vertices
  WORD*                  getIndexList();      //get the actual list of indices
  unsigned               getIndexCount();     //get 
  ID3D11Buffer*          getVertexBuffer();   //get the pointer to the vertex buffer loaded into memory
  ID3D11Buffer*          getIndexBuffer();    //get the pointer to the index buffer
  ID3D11Buffer*          getConstantBuffer(); //get the poitner to this mesh's constant buffer
  D3D_PRIMITIVE_TOPOLOGY getTopology();

  // ***** Setters ***** //
  void setTopology(D3D_PRIMITIVE_TOPOLOGY topology);    //change the vertex topology to line, point etc

  // ***** Mesh functionality ***** //

  void addTriangleLH(Vertex a, Vertex b, Vertex c);     //add trianges to an open mesh
  void addVertex(Vertex a);                             //add a vertex to an open mesh
  void closeMesh(D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);                                     //finialize the mesh (initialize the data) cannot (currently) be undone
  



  private:
    ID3D11Buffer* g_pVertexBuffer = nullptr;
    ID3D11Buffer* g_pIndexBuffer = nullptr;
    
    std::vector<Vertex> vertexList_;
    std::vector<WORD> indexList_;

    D3D_PRIMITIVE_TOPOLOGY topology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
#endif // !_Mesh.h_


