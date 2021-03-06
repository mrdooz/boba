Texture2D Texture0 : register(t0);
sampler LinearSampler : register(s0);

cbuffer PerFrame : register(b0)
{
  float4 col;
  uint4 msg[1024];
};

struct VSQuadOut
{
    float4 pos : SV_Position;
    float2 uv: TexCoord;
};

// outputs a full screen triangle with screen-space coordinates
// input: three empty vertices
VSQuadOut VsMain(uint vertexID : SV_VertexID)
{
    VSQuadOut result;
  // ID=0 -> Pos=[-1,-1], Tex=[0,0]
  // ID=1 -> Pos=[ 3,-1], Tex=[2,0]
  // ID=2 -> Pos=[-1,-3], Tex=[0,2]
    result.uv = float2((vertexID << 1) & 2, vertexID & 2);
    result.pos = float4(result.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return result;
}

float4 PsMain(VSQuadOut input) : SV_TARGET
{
  float4 background = Texture0.Sample(LinearSampler, input.uv);
  return col + input.uv.x * input.uv.y * background;
}
