cbuffer PerFrame : register(b0)
{
  matrix viewProj;
};

Texture2D Texture0 : register(t0);
sampler LinearSampler : register(s0);

struct VsIn
{
  float3 pos : POSITION;
  uint vertexId : SV_VertexID;
};

struct VsOut
{
    float4 pos : SV_Position;
    float2 uv: TexCoord;
};

VsOut VsMain(VsIn v)
{
  // 1, 2
  // 0, 3

  VsOut res;
  res.pos = mul(float4(v.pos, 1), viewProj);
  
  switch (v.vertexId % 4)
  {
    case 0: res.uv = float2(0, 1); break;
    case 1: res.uv = float2(0, 0); break;
    case 2: res.uv = float2(1, 0); break;
    case 3: res.uv = float2(1, 1); break;
  }
  return res;
}

float4 PsMain(VsOut input) : SV_TARGET
{
  return Texture0.Sample(LinearSampler, input.uv);
}
