// Fullscreen quad vertex shader

struct VSQuadOut
{
    float4 pos : SV_Position;
    float2 uv: TexCoord;
};

VSQuadOut VsMain(uint vertexID : SV_VertexID)
{
  // 1, 2
  // 0, 3
  // output verts in order 0, 1, 2, 0, 2, 3
  VSQuadOut result;
  switch (vertexID % 6)
  {
    case 0: result.pos = float4(-1, -1, 0, 1); result.uv = float2(0, 1); break;
    case 1: result.pos = float4(-1, +1, 0, 1); result.uv = float2(0, 0); break;
    case 2: result.pos = float4(+1, +1, 0, 1); result.uv = float2(1, 0); break;
    case 3: result.pos = float4(-1, -1, 0, 1); result.uv = float2(0, 1); break;
    case 4: result.pos = float4(+1, +1, 0, 1); result.uv = float2(1, 0); break;
    case 5: result.pos = float4(+1, -1, 0, 1); result.uv = float2(1, 1); break;
  }
  return result;
}
