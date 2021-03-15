//PerVertexLighting.fx 
//FirstName: Tristan 
//LastName: Wauthier 
//Class: 2DAE_GD02

//Matrix Variables
float4x4 gWorldViewProj: WORLDVIEWPROJECTION;

float4x4 gWorld : WORLD;
float3 gEyePosW;

//Color Variables
float4 gColorAmbient = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 gColorDiffuse = float4(.9f, .9f, .1f, 1.0f);
float4 gColorSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);

//Shininess
int gShininess = 15;

//Light Direction
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

//In- and Output structs 
struct VS_INPUT{
	float3 position : POSITION;
	float3 normal : NORMAL; 
	};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float2 lighting : COLOR1;
};

//Main Vertex Shader 
VS_OUTPUT MainVS(VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	//Transformations 
	//Step 1 
	float4 position = float4(input.position,1); 
	//Step 2 
	float4 worldPosition = mul(position, gWorld); 
	//Step 3 
	float3 tNormal= normalize(mul(input.normal, (float3x3)gWorld)); 
	//Step 4 
	output.position = mul(position, gWorldViewProj);
	float3 viewDirection = normalize(worldPosition.xyz - gEyePosW);
	
	//Diffuse 
	float diffuseStrength = dot(tNormal, -gLightDirection);
	
	output.lighting.x = saturate(diffuseStrength);
	//Specular 
	float3 halfVector = -normalize(viewDirection + gLightDirection);
	float specularStrength = dot(halfVector, tNormal);
	specularStrength = saturate(specularStrength);
	output.lighting.y = pow(specularStrength, gShininess);
	//Color 
	output.color = float4(0.9f,0.9f,0.9f,1); return output;
}

//Main Pixel Shader 
float4 MainPS(VS_OUTPUT input) : SV_TARGET 
{
	//Diffuse 
	float3 diffColor = input.color.rgb * gColorDiffuse.rgb;
	diffColor *= input.lighting.x; 
	//Specular 
	float3 specColor = gColorSpecular.xyz * input.lighting.y; 
	//Final Color 
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