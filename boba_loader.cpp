#include "boba_loader.hpp"
#include "scene.hpp"
#include "graphics.hpp"

using namespace boba;
using namespace bristol;

bool BobaLoader::Load(const char* filename)
{
  if (!LoadFile(filename, &buf))
    return false;

  const BobaScene* scene = (const BobaScene*)&buf[0];

  if (strncmp(scene->id, "boba", 4) != 0)
    return false;

  ProcessFixups(scene->fixupOffset);

  char* ptr;

  // add meshes
  ptr = &buf[scene->elementOffset[(u32)SceneElement::Mesh]];
  u32 numMeshes = *(u32*)ptr;
  MeshElement* element = (MeshElement*)(ptr + 4);
  for (u32 i = 0; i < numMeshes; ++i, ++element)
  {
    meshes.push_back(element);
  }
  return true;
}

bool BobaLoader::LoadMesh(const string& name, u32 flags, Mesh* mesh)
{
  for (const MeshElement* e : meshes)
  {
    if (e->name == name)
    {
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

      return true;
    }
  }

  return false;
}

void BobaLoader::ProcessFixups(u32 fixupOffset)
{
  // Process all the fixups. A list of locations that point to relative
  // data is stored (the fixup list), and for each of these locations, we
  // add the base of the file we loaded, converting the fixups to valid
  // memory locations

  // Note, on 64-bit, we are still limited to 32 bit file sizes and offsets, but
  // all the fixed up pointers are 64-bit.
  u32* fixupList = (u32*)&buf[fixupOffset];
  u32 numFixups = *fixupList++;
  intptr_t base = (intptr_t)&buf[0];
  for (u32 i = 0; i < numFixups; ++i)
  {
    u32 fixup = fixupList[i];

    // this is a bit cheesy, but the offsets are stored as 32 bits
    u32 ofs = *(u32*)(base + fixup);
    *(intptr_t*)(base + fixup) = base + ofs;
  }
}
