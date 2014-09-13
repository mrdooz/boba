#pragma once

#include "object_handle.hpp"

namespace boba
{
  struct BobaLoader;

  struct Mesh
  {
    Mesh();

    string _name;
    ObjectHandle _vb;
    ObjectHandle _ib;
    ObjectHandle _layout;
    u32 _numVerts;
    u32 _numIndices;
    BoundingSphere _boundingSphere;
    Matrix _world;
  };

  Mesh* MeshFromLoader(const BobaLoader& loader, const string& name, u32 flags);

  struct Scene
  {
    vector<Mesh> _meshes;
  };

}
