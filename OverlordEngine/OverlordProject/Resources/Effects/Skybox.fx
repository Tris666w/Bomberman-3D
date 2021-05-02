//Skybox.fx 
//FirstName: Tristan 
//LastName: Wauthier 
//Class: 2DAE02

//Matrix Variables
float4x4 gMatrixWVP: WORLDVIEWPROJECTION;
float4x4 gWorld : WORLD;

//cubemap Variables
TextureCube gTextureEnv <
string UIName = "Env Map";
string ResourceType = "Cube";
>;
SamplerState gSamplerEnvMap
{
Filter = MIN_MAG_MIP_LINEAR;
AddressU = WRAP;
AddressV = WRAP;
AddressW = WRAP;
};


//Camera pos
float3 gCameraPosition;

//Rasterizer States 
RasterizerState gRS_NoCulling
{
	CullMode = NONE;

};

//In- and Output structs
struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

//Vertex shader
VS_OUTPUT MainVS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.position = mul(float4(input.position,1.f), gMatrixWVP);
 
	float4 VertexPosition = mul(float4(input.position,1.f), gWorld);
	output.texcoord = VertexPosition.xyz - gCameraPosition;
	
	return output;
};

//PixelShader
float4 MainPS(VS_OUTPUT input) : SV_Target
{
	float4 color = gTextureEnv.Sample(gSamplerEnvMap, input.texcoord);
	return color;
};

//Default Technique for DirectX10
technique10 DefaultTechnique
{
	pass P0 
	{
		SetRasterizerState(gRS_NoCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS())); 
		SetPixelShader(CompileShader(ps_4_0, MainPS()));


	}
}