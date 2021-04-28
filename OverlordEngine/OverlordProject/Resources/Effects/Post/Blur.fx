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
	output.Position = float4(input.Position, 1.f);
	output.TexCoord = input.TexCoord;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
	uint width, height;
	gTexture.GetDimensions(width, height);
	
	// Step 2: calculate dx and dy (UV space for 1 pixel)	
	float dx = 1.f / width;
	float dy = 1.f / height;
	
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	
	//calculate topleft of ourKernel (grid) with stride of +1 (stride can never be 0)
	float stride = 2.f;
	
	float2 startUV = float2(input.TexCoord - stride * 2 * float2(dx, dy));
	
	float4 finalcolor = float4(0.f, 0.f, 0.f, 0.f);
	for (int x = 0; x < 5; ++x)
	{
		float offsetX = stride * x * dx;
		for (int y = 0; y < 5; ++y)
		{
			float offsetY = stride * y * dy;
			finalcolor += gTexture.Sample(samPoint, startUV + float2(offsetX, offsetY));
		}
	}
	
	// Step 4: Divide the final color by the number of passes (in this case 5*5)	
	finalcolor /= (5 * 5);
	
	// Step 5: return the final color
	return float4(finalcolor.rgb, 1.f);
}


//TECHNIQUE
//---------
technique11 Blur
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