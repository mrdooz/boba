#pragma once

namespace boba
{
  enum VertexFlags
  {
    VF_POS    = 1 << 0,
    VF_NORMAL = 1 << 1,
    VF_UV     = 1 << 2,
    VF_COLOR  = 1 << 3,
  };

  u32 ColorToU32(float r, float g, float b, float a);

  float Raycast(const Vector3& center, float radius, const Vector3& o, const Vector3& d);

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