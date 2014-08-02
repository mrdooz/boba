#include "scene.hpp"
#include "boba_loader.hpp"
#include "graphics.hpp"

using namespace boba;
using namespace bristol;

Mesh::Mesh()
    : _world(Matrix::Identity())
{
}

Mesh* boba::MeshFromLoader(const BobaLoader& loader, const string& name, u32 flags)
{
  for (const BobaLoader::MeshElement* e : loader.meshes)
  {
    if (e->name == name)
    {
      Mesh* mesh = new Mesh();
      mesh->_name = name;
      mesh->_numVerts = e->numVerts;
      mesh->_numIndices = e->numIndices;
      mesh->_boundingSphere = BoundingSphere(XMFLOAT3(e->bx, e->by, e->bz), e->br);

      // create a combined buffer for the vertex data
      bool hasPos = flags & VF_POS && e->verts;
      bool hasNormal = flags & VF_NORMAL && e->normals;
      bool hasUv = flags & VF_UV && e->uv;

      u32 vertexSize = (hasPos ? 3 : 0) + (hasNormal ? 3 : 0) + (hasUv ? 2 : 0);

      vector<u8> buf(vertexSize * e->numVerts * sizeof(float));
      float* ptr = (float*)buf.data();

      // interleave the vertex data..
      u32 numVerts = e->numVerts;
      u32 ofs = 0;
      if (hasPos)
      {
        for (u32 i = 0; i < numVerts; ++i)
          memcpy(&ptr[i*vertexSize], &e->verts[i*3], 3 * sizeof(float));
        ofs += 3;
      }

      if (hasNormal)
      {
        for (u32 i = 0; i < numVerts; ++i)
          memcpy(&ptr[i*vertexSize + ofs], &e->normals[i*3], 3 * sizeof(float));
        ofs += 3;
      }

      if (hasUv)
      {
        for (u32 i = 0; i < numVerts; ++i)
          memcpy(&ptr[i*vertexSize + ofs], &e->uv[i*2], 2 * sizeof(float));
        ofs += 2;
      }

      mesh->_vb = GRAPHICS.CreateBuffer(
        D3D11_BIND_VERTEX_BUFFER,
        e->numVerts * vertexSize * sizeof(float),
        false,
        buf.data(),
        vertexSize * sizeof(float));

      mesh->_ib = GRAPHICS.CreateBuffer(
        D3D11_BIND_INDEX_BUFFER,
        e->numIndices * sizeof(u32),
        false,
        e->indices,
        DXGI_FORMAT_R32_UINT);

      return mesh;
    }
  }

  return nullptr;
}
