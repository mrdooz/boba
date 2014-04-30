cbuffer PerFrame : register(b0)
{
  matrix world;
  matrix viewProj;
};

struct VsIn
{
  float3 pos : POSITION;
  float3 normal : NORMAL;
};

struct VsOut
{
  float4 pos : SV_Position;
  float3 normal : NORMAL;
};

VsOut VsMain(VsIn v)
{
  VsOut res;
  matrix worldViewProj = mul(world, viewProj);
  res.pos = mul(float4(v.pos, 1), worldViewProj);
  res.normal = mul(v.normal, (float3x3)worldViewProj);
  return res;
}

float4 PsMain(VsOut p) : SV_Target
{
  float t = 0.1 + saturate(dot(normalize(p.normal), float3(0,0,-1)));
  return t;
}
