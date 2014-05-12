cbuffer PerFrame : register(b0)
{
  matrix world;
  matrix viewProj;
};

struct VsIn
{
  float3 pos : POSITION;
  float4 color : COLOR;
};

struct VsOut
{
  float4 pos : SV_Position;
  float4 color : COLOR;
};

VsOut VsMain(VsIn v)
{
  VsOut res;
  matrix worldViewProj = mul(world, viewProj);
  res.pos = mul(float4(v.pos, 1), worldViewProj);
  res.color = v.color;
  return res;
}

float4 PsMain(VsOut p) : SV_Target
{
  return p.color;
}
