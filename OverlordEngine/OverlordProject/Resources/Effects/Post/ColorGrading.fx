//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Mirror;
	AddressV = Mirror;
};


float gContribution;
Texture2D gLUT;
Texture2D gRenderedText;
#define LUTColors 32.0f

// Create Depth Stencil State (ENABLE DEPTH WRITING)
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
	//Normal VS
	PS_INPUT output = (PS_INPUT)0;
	output.Position = float4(input.Position, 1.f);
	output.TexCoord = input.TexCoord;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	//Get the texture dimensions of the LUT
	uint width, height;
	gLUT.GetDimensions(width, height);
	
	//Calculate the texel sizes
	float dx = 1.f / width;
	float dy = 1.f / height;
	
	float maxColor = LUTColors - 1.0;
	
	//Get the color of the pixel in this renderloop (make sure it is in [0;1]!)
	float3 col = saturate(gRenderedText.Sample(samPoint, input.TexCoord).rgb);
	
	//Calculate the half of a texel and a threshold for precision
	float halfColX = 0.5 / dx;
	float halfColY = 0.5 / dy;
	float threshold = maxColor / LUTColors;
 
	//Calculate red [half texel from the left;half a texel from the right]
	float xOffset = halfColX + col.r * threshold / LUTColors;
	
	//Calculate green [half texel from the bottom;half a texel from the top]
	float yOffset = halfColY + col.g * threshold;
	
	//Calculate blue [0,maxColor]
	float cell = floor(col.b * maxColor);
 
	//New uv coords
	float2 lutPos = float2(cell / LUTColors + xOffset, yOffset);
	
	//Sample the LUT for the graded color
	float3 gradedCol = gLUT.Sample(samPoint, lutPos);
                 
	float3 finalColor = lerp(col, gradedCol, gContribution);
	
	//Interpolate the colors, based on the contributions
	return float4(finalColor, 1.f);
}


//TECHNIQUE
//---------
technique11 ColorGrading
{
	pass P0
	{
		SetRasterizerState(BackCulling);
		SetDepthStencilState(DisableDepthWriting, 0);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	
	}
}