struct VSQuadOut
{
    float4 position : SV_Position;
    float2 uv: TexCoord;
};

// outputs a full screen triangle with screen-space coordinates
// input: three empty vertices
VSQuadOut VSQuad(uint vertexID : SV_VertexID)
{
    VSQuadOut result;
	// ID=0 -> Pos=[-1,-1], Tex=[0,0]
	// ID=1 -> Pos=[ 3,-1], Tex=[2,0]
	// ID=2 -> Pos=[-1,-3], Tex=[0,2]
    result.uv = float2((vertexID << 1) & 2, vertexID & 2);
    result.position = float4(result.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return result;
}

float4 PSQuad(VSQuadOut input) : SV_TARGET
{
	return input.uv.x * input.uv.y;
}
