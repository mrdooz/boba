cbuffer PerFrame : register(b0)
{
  matrix world;
  matrix viewProj;
};

struct VsIn
{
  float3 pos : POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

struct VsOut
{
  float4 pos : SV_Position;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

VsOut VsMain(VsIn v)
{
  VsOut res;
  matrix worldViewProj = mul(world, viewProj);
  res.pos = mul(float4(v.pos, 1), worldViewProj);
  res.normal = mul(v.normal, (float3x3)world);
  res.color = v.color;
  return res;
}

Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1);
Texture2D Texture2 : register(t2);
sampler PointSampler : register(s0);
sampler LinearSampler : register(s1);

struct PSInput
{
  float4 pos : SV_Position;
  float2 uv: TexCoord;
};

cbuffer textureSize : register(b0)
{
  float2 inputSize : packoffset(c0.x);
  float2 outputSize : packoffset(c0.z);
};

//------------------------------------------------------------------------------
float Luminance(float3 c)
{
  return 0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z;
}

//------------------------------------------------------------------------------
float4 PsMain(VsOut p) : SV_Target
{
  //return p.color;
  return p.color * 0.5 * (0.2 + saturate(dot(normalize(p.normal), float3(0,-1,-1))));
}

//------------------------------------------------------------------------------
float4 EdgeDetect(in PSInput input) : SV_Target
{

  //float3 color = Texture0.Sample(PointSampler, input.uv).rgb;
  //return float4(color, 1);

  // Sobel filter
  float2 ofs = 1 / inputSize;
  float3 l00 = Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, -ofs.y)).rgb;
  float3 l01 = Texture0.Sample(PointSampler, input.uv + float2(     0, -ofs.y)).rgb;
  float3 l02 = Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, -ofs.y)).rgb;

  float3 l10 = Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, 0)).rgb;
  float3 l11 = Texture0.Sample(PointSampler, input.uv + float2(     0, 0)).rgb;
  float3 l12 = Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, 0)).rgb;

  float3 l20 = Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, +ofs.y)).rgb;
  float3 l21 = Texture0.Sample(PointSampler, input.uv + float2(     0, +ofs.y)).rgb;
  float3 l22 = Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, +ofs.y)).rgb;
  
  float3 gx = +1 * l00 -1 * l02 +2 * l10 -2 * l12 +1 * l20 -1 * l22;
  float3 gy = +1 * l00 +2 * l01 +1 * l02 -1 * l20 -2 * l21 -1 * l22;
  
  //return float4(l11, 1);
  float3 e = sqrt(gx*gx+gy*gy);
  float t = max(e.x, e.y);
  //return t * float4(0.3 * t * t, 0.9 * t, 0.1, 1);

  return t > 0.3 ? 1 : 0;
  
  
//  float3 color = Texture0.Sample(PointSampler, input.uv).rgb;

//  return dot(color, float3(1,1,1));
}

//------------------------------------------------------------------------------
float4 EdgeDetect2(in PSInput input) : SV_Target
{

  //float3 color = Texture0.Sample(PointSampler, input.uv).rgb;
  //return float4(color, 1);

  // Sobel filter
  float2 ofs = 1 / inputSize;
  float l00 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, -ofs.y)).rgb);
  float l01 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(     0, -ofs.y)).rgb);
  float l02 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, -ofs.y)).rgb);

  float l10 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, 0)).rgb);
  float l11 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(     0, 0)).rgb);
  float l12 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, 0)).rgb);

  float l20 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(-ofs.x, +ofs.y)).rgb);
  float l21 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(     0, +ofs.y)).rgb);
  float l22 = Luminance(Texture0.Sample(PointSampler, input.uv + float2(+ofs.x, +ofs.y)).rgb);
  
  float gx = +1 * l00 -1 * l02 +2 * l10 -2 * l12 +1 * l20 -1 * l22;
  float gy = +1 * l00 +2 * l01 +1 * l02 -1 * l20 -2 * l21 -1 * l22;
  
  float e = sqrt(gx*gx+gy*gy);
  //return e > 0.2 ? 1 : e;
  
  
//  float3 color = Texture0.Sample(PointSampler, input.uv).rgb;

//  return dot(color, float3(1,1,1));
}
