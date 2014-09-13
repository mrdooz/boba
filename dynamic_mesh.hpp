#pragma once

#include "object_handle.hpp"


namespace boba
{
  struct DynamicMesh
  {
    struct Face
    {
      Face() {}
      Face(u32 a, u32 b, u32 c) : a(a), b(b), c(c) {}
      u32 a, b, c;
      Vector3 normal;
    };

    DynamicMesh();

    void Init();
    void Reset();

    Matrix World();

    void CalcBoundingSphere();

    vector<float> verts;
    vector<u32> indices;
    vector<Face> faces;

    Vector3 center;
    float radius;

    Matrix rotation;
    Vector3 translation;

    ObjectHandle _wireframe;
    ObjectHandle _solid;
  };

  void SetDynamicMesh(DynamicMesh* mesh);

  int __cdecl luaopen_meshlib(lua_State* L);
  int __cdecl luaopen_mesh(lua_State* L);
}
