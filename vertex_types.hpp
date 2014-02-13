#pragma once

// Some common vertex types collected in one place..
namespace boba
{
  struct TangentSpace
  {
    TangentSpace() {}
    TangentSpace(const Vector3 &tangent, const Vector3& normal, const Vector3 &bitangent)
      : tangent(tangent), normal(normal), bitangent(bitangent) {}
    TangentSpace(float tx, float ty, float tz, float nx, float ny, float nz, float bx, float by, float bz) 
      : tangent(tx, ty, tz), normal(nx, ny, nz), bitangent(bx, by, bz) {}
    Vector3 tangent, normal, bitangent;
  };


  struct PosNormal
  {
    PosNormal() {}
    PosNormal(const Vector3& pos, const Vector3& normal) : pos(pos), normal(normal) {}
    PosNormal(float x, float y, float z, float nx, float ny, float nz) : pos(x, y, z), normal(nx, ny, nz) {}
    Vector3 pos;
    Vector3 normal;
  };

  struct PosTangentSpace
  {
    PosTangentSpace() {}
    PosTangentSpace(const Vector3& pos, const Vector3& normal, const Vector3 &tangent, const Vector3 &binormal)
      : pos(pos), normal(normal), tangent(tangent), binormal(binormal){}
    Vector3 pos;
    Vector3 normal, tangent, binormal;
  };

  struct PosTangentSpaceTex
  {
    PosTangentSpaceTex() {}
    PosTangentSpaceTex(const Vector3& pos, const Vector3& normal, const Vector3 &tangent, const Vector3 &binormal, const Vector2 &tex)
      : pos(pos), normal(normal), tangent(tangent), binormal(binormal), tex(tex) {}
    Vector3 pos;
    Vector3 normal, tangent, binormal;
    Vector2 tex;
  };

  // bitangent = cross(normal, tangent)
  struct PosTangentSpace2
  {
    PosTangentSpace2() {}
    PosTangentSpace2(const Vector3& pos, const Vector2 &tex, const Vector3 &tangent, const Vector3& normal) 
      : pos(pos), tex(tex), tangent(tangent), normal(normal) {}
    Vector3 pos;
    Vector2 tex;
    Vector3 tangent, normal;
  };

  struct Pos2Tex
  {
    Pos2Tex() {}
    Pos2Tex(const Vector2& pos, const Vector2& tex) : pos(pos), tex(tex) {}
    Pos2Tex(float x, float y, float u, float v) : pos(x, y), tex(u, v) {}
    Vector2 pos;
    Vector2 tex;
  };

  struct PosTex
  {
    PosTex() {}
    PosTex(const Vector3& pos, const Vector2& tex) : pos(pos), tex(tex) {}
    PosTex(float x, float y, float z, float u, float v) : pos(x, y, z), tex(u, v) {}
    Vector3 pos;
    Vector2 tex;
  };

  struct PosColTex
  {
    PosColTex() {}
    PosColTex(const Vector3& pos, const Vector4& col, const Vector2& tex) : pos(pos), col(col), tex(tex) {}
    PosColTex(float x, float y, float z, float r, float g, float b, float a, float u, float v) : pos(x, y, z), col(r, g, b, a), tex(u, v) {}
    Vector3 pos;
    Vector4 col;
    Vector2 tex;
  };

  struct Pos4Tex
  {
    Pos4Tex() {}
    Pos4Tex(const Vector4 &pos, const Vector2& tex) : pos(pos), tex(tex) {}
    Pos4Tex(float x, float y, float z, float w, float u, float v) : pos(x, y, z, w), tex(u, v) {}
    Vector4 pos;
    Vector2 tex;
  };

  struct PosCol
  {
    PosCol() {}
    PosCol(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), col(r, g, b, a) {}
    PosCol(float x, float y, float z, const Vector4& col) : pos(x, y, z), col(col) {}
    PosCol(const Vector3& pos, const Vector4& col) : pos(pos), col(col) {}
    PosCol(const Vector2& pos, float z, const Vector4& col) : pos(pos.x, pos.y, z), col(col) {}
    PosCol(const Vector3& pos) : pos(pos) {}
    Vector3 pos;
    Vector4 col;
  };

  struct PosNormalTex
  {
    PosNormalTex() {}
    PosNormalTex(const Vector3& pos, const Vector3& normal, const Vector2& tex) : pos(pos), normal(normal), tex(tex) {}
    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
  };
}
