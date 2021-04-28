//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Mirror;
	AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState DisableDepthWriting
{
	DepthEnable = true;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState BackCulling
{
	CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	// Set the TexCoord
	output.Position = float4(input.Position, 1.f);
	output.TexCoord = input.TexCoord;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: sample the texture
	// Step 2: calculate the mean value
	// Step 3: return the color
	float4 color = gTexture.Sample(samPoint, input.TexCoord);
	float average = color.r * 0.3 + color.g * 0.59 + color.b * 0.11;
	float4 result = float4(average, average, average, 1.f);
	return result;
}


//TECHNIQUE
//---------
technique11 Grayscale
{
	pass P0
	{          
		// Set states...
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );

		SetRasterizerState(BackCulling);
		SetDepthStencilState(DisableDepthWriting, 0);
	}
}

