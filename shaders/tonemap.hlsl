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
  // texture_1 = log(avg) luminance (there is no adaption phase, so this is the full 1024x1024
  //             texture).
  
  // sample the log(avg), and convert to avg luminance
  float avgLuminance = exp(Texture1.SampleLevel(PointSampler, float2(0.5f, 0.5f), 10.0f).x);
  float3 color = Texture0.Sample(PointSampler, input.uv).rgb;

  color = ToneMap(color, avgLuminance, 0.18);
  return float4(color, 1.0f);
}
