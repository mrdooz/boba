#pragma once

#include "graphics_object_handle.hpp"

namespace boba
{
  struct Mesh
  {
    Mesh();

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
