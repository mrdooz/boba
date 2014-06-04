#include "graphics_utils.hpp"
#include "graphics.hpp"
#include "scene.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  float Raycast(const Vector3& center, float radius, const Vector3& o, const Vector3& d)
  {
    // ray -> sphere intersection
    // Compute A, B and C coefficients
    float a = Dot(d, d);
    float b = 2 * Dot(d, o - center);
    float c = Dot(o - center, o - center) - (radius * radius);

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
    GRAPHICS.GetBackBufferSize(&w, &h);
    float xv = (2.0f * x / w - 1) / proj(0, 0);
    float yv = (-2.0f * y / h + 1) / proj(1, 1);

    return Vector3(xv, yv, 1);
  }

  //------------------------------------------------------------------------------
  u32 ColorToU32(float r, float g, float b, float a)
  {
    return 
      ((u32)(255 * clamp(0.0f, 1.0f, r)) << 0) +
      ((u32)(255 * clamp(0.0f, 1.0f, g)) << 8) +
      ((u32)(255 * clamp(0.0f, 1.0f, b)) << 16) +
      ((u32)(255 * clamp(0.0f, 1.0f, a)) << 24);
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
        u32 ofs = 0;
        if (vertexFlags & VF_POS)
        {
          D3D11_INPUT_ELEMENT_DESC element =
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
          desc.push_back(element);
          ofs += 12;
        }

        if (vertexFlags & VF_NORMAL)
        {
          D3D11_INPUT_ELEMENT_DESC element =
          { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, ofs, D3D11_INPUT_PER_VERTEX_DATA, 0 };
          desc.push_back(element);
          ofs += 12;
        }

        if (vertexFlags & VF_COLOR)
        {
          D3D11_INPUT_ELEMENT_DESC element =
          { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, ofs, D3D11_INPUT_PER_VERTEX_DATA, 0 };
          desc.push_back(element);
          ofs += 16;
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


/*
  void CreateCylinder(
      float width,
      float height,
      float length,
      u32 heightSegments,
      u32 radialSegments,
      Mesh* mesh)
  {
    float y = 0;
    for (u32 i = 0; i < heightSegments+1; ++i)
    {
      float inc = 2 * DirectX::XM_PI / radialSegments;
      float angle = 0;
      for (u32 j = 0; j < radialSegments; ++j)
      {
        // create a ring
        float x = width * cos(angle);
        float z = width * sin(angle);
        angle += inc;

        mesh->verts.push_back(PosNormal(x, y, z, cos(angle), 0, sin(angle)));

        if (i < heightSegments)
        {
          // c-d
          // |\|
          // a-b

          u32 a = (i + 0) * radialSegments + j;
          u32 b = (i + 0) * radialSegments + (j + 1) % radialSegments;
          u32 c = (i + 1) * radialSegments + j;
          u32 d = (i + 1) * radialSegments + (j + 1) % radialSegments;

          mesh->faces.push_back(Face(a, c, b));
          mesh->faces.push_back(Face(b, c, d));

          mesh->indices.push_back(a);
          mesh->indices.push_back(c);
          mesh->indices.push_back(b);

          mesh->indices.push_back(b);
          mesh->indices.push_back(c);
          mesh->indices.push_back(d);

        }
      }
      y += height / heightSegments;
    }
  }
*/
