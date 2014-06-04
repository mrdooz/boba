Texture2D Texture0 : register(t0);
sampler PointSampler : register(s0);

struct VSQuadOut
{
    float4 pos : SV_Position;
    float2 uv: TexCoord;
};

float4 PsMain(VSQuadOut input) : SV_TARGET
{
  return Texture0.Sample(PointSampler, input.uv);
}
