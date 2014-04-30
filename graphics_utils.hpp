#pragma once

namespace boba
{
  struct Mesh;
  float Raycast(const Mesh& mesh, const Vector3& o, const Vector3& d);

  Vector3 ScreenToViewSpace(const Matrix& proj, u32 x, u32 y);

  template <typename T>
  float Dot(const T& a, const T& b)
  {
    return a.Dot(b);
  }

  class GraphicsObjectHandle;
  bool LoadShadersFromFile(
    const string& filenameBase,
    GraphicsObjectHandle* vs,
    GraphicsObjectHandle* ps,
    GraphicsObjectHandle* inputLayout,
    u32 vertexFlags);

}