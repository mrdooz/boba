// Tonemapping, using http://mynameismjp.wordpress.com/2011/08/10/average-luminance-compute-shader/
// as a reference

Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1);
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

cbuffer settings : register(b1)
{
  float tua : packoffset(c0.x);
  float key : packoffset(c0.y);
  float timeDelta : packoffset(c0.z);
};

//------------------------------------------------------------------------------
// Approximates luminance from an RGB value
float CalcLuminance(float3 color)
{
  return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}

//------------------------------------------------------------------------------
// Retrieves the log-average luminance from the texture
float GetAvgLuminance(Texture2D lumTex)
{
  return lumTex.Load(uint3(0, 0, 0)).x;
}

//------------------------------------------------------------------------------
// Reinhart tonemapping
float3 ToneMap(float3 color, float avgLuminance, float key)
{
  return key / avgLuminance * color;
}

//------------------------------------------------------------------------------
// Slowly adjusts the scene luminance based on the previous scene luminance
float4 AdaptLuminance(in PSInput input) : SV_Target
{
  float lastLum = Texture0.Load(uint3(0, 0, 0)).x;
  float currentLum = exp(Texture1.SampleLevel(PointSampler, float2(0.5f, 0.5f), 10.0f).x);

  // Adapt the luminance using Pattanaik's technique
  float adaptedLum = lastLum + (currentLum - lastLum) * (1 - exp(-timeDelta * tua));

  return float4(adaptedLum, 1.0f, 1.0f, 1.0f);
}

//------------------------------------------------------------------------------
// Creates the luminance map for the scene
float4 LuminanceMap(in PSInput input) : SV_Target
{
  // Sample the input

  float3 color = Texture0.Sample(LinearSampler, input.uv).rgb;
  // calculate the luminance using a weighted average
  float luminance = log(max(CalcLuminance(color), 0.00001f));
  
  return float4(luminance, 1.0f, 1.0f, 1.0f);
}

//------------------------------------------------------------------------------
// Apply tonemapping
float4 Composite(in PSInput input) : SV_Target
{
  // texture_0 = input texture
  // texture_1 = avg luminance
  
  float3 color = Texture0.Sample(PointSampler, input.uv).rgb;
  float avgLuminance = GetAvgLuminance(Texture1);
  color = ToneMap(color, avgLuminance, key);
  return float4(color, 1.0f);
}
