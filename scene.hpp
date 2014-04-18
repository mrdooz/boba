#pragma once

#include "graphics_object_handle.hpp"

namespace boba
{
  struct Mesh
  {
    Mesh();

    enum VertexFlags
    {
      VF_POS = 1 << 0,
      VF_NORMAL = 1 << 1,
      VF_UV = 1 << 2,
    };

    string _name;
    GraphicsObjectHandle _vb;
    GraphicsObjectHandle _ib;
    GraphicsObjectHandle _layout;
    u32 _numVerts;
    u32 _numIndices;
    BoundingSphere _boundingSphere;
    Matrix _world;
  };

  struct Scene
  {
    vector<Mesh> _meshes;
  };

}
