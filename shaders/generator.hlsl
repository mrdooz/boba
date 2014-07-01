cbuffer PerFrame : register(b0)
{
  matrix world;
  matrix viewProj;
};

struct VsIn
{
  float3 pos : POSITION;
  float3 normal : NORMAL;
  uint idx : SV_VertexId;
};

struct VsOut
{
  float4 pos : SV_Position;
  float3 normal : NORMAL;
  noperspective float3 dist : COLOR;
};

VsOut VsMain(VsIn v)
{
  VsOut res;
  matrix worldViewProj = mul(world, viewProj);
  res.pos = mul(float4(v.pos, 1), worldViewProj);
  res.normal = mul(v.normal, (float3x3)world);
  switch (v.idx % 3)
  {
    case 0: res.dist = float3(0,0,1); break;
    case 1: res.dist = float3(0,1,0); break;
    case 2: res.dist = float3(1,0,0); break;
  }
  return res;
}

float4 PsMain(VsOut p) : SV_Target
{
  float d = min(p.dist.x, min(p.dist.y, p.dist.z));
  float t;
  if (d < 0.02)
    t = 1;
  else
    t = 0.1 + 0.5 * saturate(dot(normalize(p.normal), float3(0,-1,0)));
  return float4(t*0.3, t*0.6, t*0.3, 1);
}
