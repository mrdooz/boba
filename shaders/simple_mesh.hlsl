cbuffer PerFrame : register(b0)
{
  float4x4 world;
  float4x4 viewProj;
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
  float4x4 worldViewProj = world * viewProj;
  res.pos = mul(float4(v.pos, 1), worldViewProj);
  res.normal = mul(v.normal, (float3x3)worldViewProj);
  return res;
}

float4 PsMain(VsOut p) : SV_Target
{
  return float4(1,0,1,1);
}
