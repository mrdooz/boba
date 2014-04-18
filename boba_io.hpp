#pragma once

typedef unsigned int u32;

namespace boba
{
  enum class SceneElement
  {
    Mesh,
    Camera,
    Light,
    NumElements
  };

  struct ElementChunk
  {
    u32 numElements;
#pragma warning(suppress: 4200)
    char data[0];
  };

  struct MeshElement
  {
    const char* name;
    u32 numVerts;
    u32 numIndices;
    float* verts;
    float* normals;
    float* uv;
    u32* indices;
    // bounding sphere
    float bx, by, bz, br;
  };

  struct BobaScene
  {
    char id[4];
    u32 fixupOffset;
    u32 elementOffset[SceneElement::NumElements];
  };
}
