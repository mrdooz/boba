#include "graphics_utils.hpp"
#include "graphics.hpp"
#include "scene.hpp"
#include "resource_manager.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  float Raycast(const Mesh& mesh, const Vector3& o, const Vector3& d)
  {
    // ray is in view space
    XMFLOAT3 xx = mesh._boundingSphere.Center;
    Vector3 ro(xx.x, xx.y, xx.z);
    float r = mesh._boundingSphere.Radius;

    // ray -> sphere intersection
    // Compute A, B and C coefficients
    float a = Dot(d, d);
    float b = 2 * Dot(d, o - ro);
    float c = Dot(o - ro, o - ro) - (r * r);

    // Find discriminant
    float disc = b * b - 4 * a * c;

    // if discriminant is negative there are no real roots, so ray misses sphere
    if (disc < 0)
      return -1;

    float t0 = (-b - sqrt(disc)) / (2 * a);
    float t1 = (-b + sqrt(disc)) / (2 * a);

    // return smallest non-negative 
    float ta = min(t0, t1);
    float tb = max(t0, t1);
    return ta < 0 ? tb : ta;
  }

  //------------------------------------------------------------------------------
  Vector3 ScreenToViewSpace(const Matrix& proj, u32 x, u32 y)
  {
    int w, h;
    GRAPHICS.ScreenSize(&w, &h);
    float xv = (2.0f * x / w - 1) / proj(0, 0);
    float yv = (-2.0f * y / h + 1) / proj(1, 1);

    return Vector3(xv, yv, 1);
  }

  //------------------------------------------------------------------------------
  bool LoadShadersFromFile(
    const string& filenameBase,
    GraphicsObjectHandle* vs,
    GraphicsObjectHandle* ps,
    GraphicsObjectHandle* inputLayout,
    u32 vertexFlags)
  {
    vector<char> buf;
    if (vs)
    {
      if (!RESOURCE_MANAGER.LoadFile((filenameBase + ".vso").c_str(), &buf))
        return false;

      *vs = GRAPHICS.CreateVertexShader(buf, "VsMain");
      if (!vs->IsValid())
        return false;

      if (inputLayout)
      {
        vector<D3D11_INPUT_ELEMENT_DESC> desc;
        if (vertexFlags & Mesh::VF_POS)
        {
          D3D11_INPUT_ELEMENT_DESC element =
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
          desc.push_back(element);
        }

        if (vertexFlags & Mesh::VF_NORMAL)
        {
          D3D11_INPUT_ELEMENT_DESC element =
          { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
          desc.push_back(element);
        }

        *inputLayout = GRAPHICS.CreateInputLayout(desc, buf);
        if (!inputLayout->IsValid())
          return false;
      }
    }

    if (ps)
    {
      if (!RESOURCE_MANAGER.LoadFile((filenameBase + ".pso").c_str(), &buf))
        return false;

      *ps = GRAPHICS.CreatePixelShader(buf, "PsMain");
      if (!ps->IsValid())
        return false;
    }

    return true;
  }

}
