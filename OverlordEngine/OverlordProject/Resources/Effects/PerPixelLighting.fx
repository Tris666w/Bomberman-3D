//PerPixelLighting.fx 
//FirstName: Tristan 
//LastName: Wauthier
//Class: 2DAE_GD02

//Matrix Variables
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorld : WORLD;
float3 gEyePosW;

//Color Variables
float4 gColorAmbient = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 gColorDiffuse = float4(.9f, .9f, .1f, 1.0f);
float4 gColorSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);

//Shininess
int gShininess = 15;

//Light Direction
float3 gLightDirection =
float3(-0.577f, -0.577f, 0.577f);

//In- and Output structs 
struct VS_INPUT
{ 
	float3 position : POSITION;
	float3 normal : NORMAL; 
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float4 worldPosition : COLOR1;
	float3 normal : NORMAL;
};

//Main Vertex Shader 
VS_OUTPUT MainVS(VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0; 
	//Step 1+2 
	output.worldPosition = mul(float4(input.position,1), gWorld); 
	//Step 2 
	output.position = mul(float4(input.position,1), gWorldViewProj); 
	//Step 3 
	output.normal = normalize(mul(input.normal, (float3x3)gWorld)); 
	//Color 
	output.color = float4(0.9f,0.9f,0.9f,1); return output; 
}

//Main Pixel Shader
float4 MainPS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 viewDirection = normalize(input.worldPosition.xyz - gEyePosW);
	
	//Diffuse 
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = saturate(diffuseStrength);
	float3 diffColor = (input.color.rgb * gColorDiffuse.rgb) * diffuseStrength;
	
	//Specular 
	float3 halfVector = -normalize(viewDirection + gLightDirection);
	float specularStrength = dot(halfVector, input.normal);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength, gShininess);
	float3 specColor = gColorSpecular.rgb * specularStrength;
	float3 finalColor = diffColor + specColor + gColorAmbient.rgb;
	return float4(finalColor, 1);
}

technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}