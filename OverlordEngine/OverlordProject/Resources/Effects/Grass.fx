//Mostly based on  this paper by Nick Vanheer
//http://www.nickvanheer.com/project/NickVanheer_GrassGeometryShader.pdf

cbuffer cbPerObject
{
	float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
	float4x4 m_MatrixViewInv : VIEWINVERSE;
	float4x4 m_MatrixWorld : WORLD;
	float3 gLightDirection = { 0.2f, -1.0f, 0.2f };
}

RasterizerState DisableCulling
{
	CullMode = None;
	FillMode = Solid;
};

SamplerState m_TextureSampler
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};
SamplerState samgHeightMap
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp; // of Mirror of Clamp of Border
	AddressV = Clamp; // of Mirror of Clamp of Border
};

bool IsDense = true;
float gWindVelocity = 0.7;
float3 gWindDirection = float3(1.f, 0.f, 0.f);
float gElapsedSec : TIME;
	
Texture2D gGrassTexture;
Texture2D gHeightMap;
Texture2D gDirectionTexture;
Texture2D gWeightMap;
Texture2D gAddColorMap;

int maxHeight = 3.5;
float maxWidth = 0.5;
float unitHeightSegmentBottom = 0.4;
float unitHeightSegmentMid = 0.4;
float unitHeightSegmentTop = 0.2;

float unitWidthSegmentBottom = 0.5;
float unitWidthSegmentMid = 0.4;
float unitWidthSegmentTop = 0.2;

float bendSegmentBottom = 0.5;
float bendSegmentMid = 1;
float bendSegmentTop = 2;

bool gUseDiffuseTexture = true;
bool gUseAddedColorTexture = true;
bool gUseHeightMap = true;
bool gUseWeightMap = true;
bool gUseDirectionTexture = true;



//**********
// STRUCTS *
//**********
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 worldPosition : COLOR0;
};

struct GS_DATA
{
	
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

struct TRIANGLE
{
	float3 position0;
	float3 position1;
	float3 position2;
	
	float3 normal0;
	float3 normal1;
	float3 normal2;
};

struct TRIANGLECOLLECTION
{
	TRIANGLE triangle0;
	TRIANGLE triangle1;
	TRIANGLE triangle2;
};

//Functions
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord, int channel = 1)
{
	GS_DATA temp = (GS_DATA) 0;
	temp.Position = mul(float4(pos.xyz, 1.0f), m_MatrixWorldViewProj);
	temp.Normal = mul(normal, (float3x3) m_MatrixWorld);
	temp.TexCoord = texCoord;
	triStream.Append(temp);
}

void CreateGrass(inout TriangleStream<GS_DATA> triStream, float height, float direction, float3 pos0, float3 pos1, float3 pos2, float3 normal, float3 normal2, float3 normal3, float weight, float weight2, float weight3)
{
	//Calculate basepoints to start at
	float3 basePoint = (pos0 + pos1 + pos2) / 3;
	float3 normalbasepoint = (normal + normal2 + normal3) / 3;
	float baseWeight = (weight + weight2 + weight3) / 3.f;
	
	//Calculate segment height, width and total height, width
	float grassHeight = height * maxHeight;
	float segmentBottomHeight = grassHeight * unitHeightSegmentBottom;
	float segmentMidHeight = grassHeight * unitHeightSegmentMid;
	float segmentTopHeight = grassHeight * unitHeightSegmentTop;
	
	float grassWidth = maxWidth;
	float segmentBottomWidth = grassWidth * unitWidthSegmentBottom;
	float segmentMidWidth = grassWidth * unitWidthSegmentMid;
	float segmentTopWidth = grassWidth * unitWidthSegmentTop;

	//initial direction in which to generate the grass blades

	direction -= -0.5; //make direction range from [0,1] to [-0.5, 0.5] 
	float3 grassDirection = normalize((pos2 - pos0) * direction);
	
	//calculate the positions for each vertex
	float3 v[7]; //trianglestrip
	v[0] = basePoint - grassDirection * segmentBottomWidth;
	v[1] = basePoint + grassDirection * segmentBottomWidth;
	v[2] = basePoint - (grassDirection * segmentMidWidth) + (segmentBottomHeight * normalbasepoint);
	v[3] = basePoint + (grassDirection * segmentMidWidth) + (segmentBottomHeight * normalbasepoint);
	
	v[4] = v[3] - ((grassDirection) * segmentTopWidth) + (segmentMidHeight * normalbasepoint);
	v[5] = v[3] + ((grassDirection) * segmentTopWidth) + (segmentMidHeight * normalbasepoint);
	v[6] = v[5] + ((grassDirection) * segmentTopWidth) + (segmentTopHeight * normalbasepoint);

	//apply wind in the same direction for each grass blade
	v[2] += gWindDirection * baseWeight * ((gWindVelocity * bendSegmentBottom) * sin(gElapsedSec));
	v[3] += gWindDirection * baseWeight * ((gWindVelocity * bendSegmentBottom) * sin(gElapsedSec));
	v[4] += gWindDirection * baseWeight * ((gWindVelocity * bendSegmentMid) * sin(gElapsedSec));
	v[5] += gWindDirection * baseWeight * ((gWindVelocity * bendSegmentMid) * sin(gElapsedSec));
	v[6] += gWindDirection * baseWeight * ((gWindVelocity * bendSegmentTop) * sin(gElapsedSec));
	
	//create the vertices with a helper method
	CreateVertex(triStream, v[0], float3(0, 0, 0), float2(0, 0));
	CreateVertex(triStream, v[1], float3(0, 0, 0), float2(0.5, 0));
	CreateVertex(triStream, v[2], float3(0, 0, 0), float2(0.3, 0.3));
	CreateVertex(triStream, v[3], float3(0, 0, 0), float2(0.6, 0.3));
	
	CreateVertex(triStream, v[4], float3(0, 0, 0), float2(0.6, 0.3));
	CreateVertex(triStream, v[5], float3(0, 0, 0), float2(0.9, 0.6));
	CreateVertex(triStream, v[6], float3(0, 0, 0), float2(1, 1));

	triStream.RestartStrip();
}

void CreateTallGrass(inout TriangleStream<GS_DATA> triStream, float height, float3 pos, float3 normal, float3 normal2, float3 normal3, float2 texCoord)
{
	triStream.RestartStrip();
	float3 normalbasepoint = normal;
	float grassHeight = height * maxHeight;
	float segment1Height = grassHeight * unitHeightSegmentBottom;
	float segment2Height = grassHeight * unitHeightSegmentMid;
	float segment3Height = grassHeight * unitHeightSegmentTop;
	
	float segment1offset = 2;
	float segment2offset = 3;
	float segment3offset = 5;

	float grassWidth = 1 * maxWidth;
	
	float segment1width = grassWidth * unitWidthSegmentBottom;
	float segment2width = grassWidth * unitWidthSegmentMid;
	float segment3width = grassWidth * unitWidthSegmentTop;
	
	float3 left = pos;
	float3 right = pos + float3(grassWidth, 0, 0);
	float3 top = pos + float3(0, grassHeight, 0);
	float3 top2 = pos + float3(grassWidth, grassHeight, 0);
	
	float3 grassNormal = cross(left - top, right - top);
	
	int direction = 1;
	
	if (height > 0.5)
		direction = 1;
	else
		direction = -1;
		
	if (height > 0.5)
	{
		segment1offset = -segment1offset;
		segment2offset = -segment2offset;
		segment3offset = -segment3offset;
	}
	
	segment1offset *= (sin(gElapsedSec) / 2);
	segment2offset *= (sin(gElapsedSec) / 2);
	segment3offset *= (sin(gElapsedSec) / 2);
	
	float3 v[7]; //trianglestrip
	v[0] = pos;
	v[1] = pos + float3(segment1width, 0, 0);
	v[2] = pos + float3(0 + segment1offset, segment1Height, segment1offset * direction);
	v[3] = pos + float3(segment2width + segment1offset, segment1Height, segment1offset * direction);
	
	//part two
	v[4] = pos + float3(0 + segment1offset + segment2offset, segment1Height + segment2Height, (segment1offset + segment2offset) * direction);
	v[5] = pos + float3(segment3width + segment1offset + segment2offset, segment1Height + segment2Height, (segment1offset + segment2offset) * direction);
	
	//part 3
	v[6] = pos + float3(segment1offset + segment2offset + segment3offset, segment1Height + segment2Height + segment3Height, (segment1offset + segment2offset + segment3offset) * direction);
	
	CreateVertex(triStream, v[0], grassNormal, float2(0, 0));
	CreateVertex(triStream, v[1], grassNormal, float2(0.5, 0));
	CreateVertex(triStream, v[2], grassNormal, float2(0.3, 0.3));
	CreateVertex(triStream, v[3], grassNormal, float2(0.6, 0.3));
	CreateVertex(triStream, v[4], grassNormal, float2(0.6, 0.6));
	CreateVertex(triStream, v[5], grassNormal, float2(0.9, 0.6));
	CreateVertex(triStream, v[6], grassNormal, float2(1, 1));

	triStream.RestartStrip();
}

void CreateSmallGrass(inout TriangleStream<GS_DATA> triStream, float height, float3 pos, float3 normal, float2 texCoord)
{
	triStream.RestartStrip();
	float3 normalbasepoint = normal;

	
	float grassHeight = height * maxHeight;
	float segment1Height = grassHeight / 5;
	float segment2Height = grassHeight / 5;
	float segment3Height = grassHeight / 5;
	
	float segment1offset = 2;
	float segment2offset = 3;
	float segment3offset = 5;
	
	float grassWidth = 3.1;
	
	float segment1width = grassWidth * 0.6;
	float segment2width = grassWidth * 0.3;
	float segment3width = grassWidth * 0.2;
	
	int direction = 1;
	int depthoffset = segment3offset;
	
	if (height > 0.5)
		direction = 1;
	else
		direction = -1;
		
	
	if (height > 0.5)
	{
		segment1offset = -segment1offset;
		segment2offset = -segment2offset;
		segment3offset = -segment3offset;
	}
	
	segment1offset *= (sin(gElapsedSec) / 2);
	segment2offset *= (sin(gElapsedSec) / 2);
	segment3offset *= (sin(gElapsedSec) / 2);
	
	float3 v[6]; //trianglestrip
	v[0] = pos + float3(-segment1width, segment1Height, 0);
	v[1] = pos + float3(0, segment1Height * 0.6, 0);
	v[2] = pos; //root
	v[3] = pos + float3(segment1width, 0, 0);
	
	//part two
	v[4] = pos + float3(segment1width, segment2Height, 0); //maybe change z here for z-fighting avoiding
	v[5] = pos + float3(segment1width + segment2width, segment2Height * 1.3, 0);
	
	CreateVertex(triStream, v[0], cross(v[2] - v[0], v[1] - v[0]), float2(0.0, 0.4));
	CreateVertex(triStream, v[1], cross(v[0] - v[1], v[3] - v[1]), float2(0.2, 0.2));
	CreateVertex(triStream, v[2], cross(v[0] - v[2], v[3] - v[2]), float2(0, 0));
	CreateVertex(triStream, v[3], cross(v[2] - v[3], v[1] - v[3]), float2(0, 0));
		
	CreateVertex(triStream, v[4], cross(v[2] - v[4], v[5] - v[4]), float2(0.5, 0.5));
	CreateVertex(triStream, v[5], cross(v[3] - v[5], v[4] - v[5]), float2(0.8, 0.8));
	triStream.RestartStrip();
	
}

VS_OUTPUT MainVS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	
	output.Position = float4(input.Position, 1);
	output.TexCoord = input.TexCoord;
	output.Normal = mul(input.Normal, (float3x3) m_MatrixWorld);
	output.Normal = normalize(output.Normal);

	output.worldPosition = mul(float4(input.Position, 1.f), m_MatrixWorld);

	return output;
}

TRIANGLECOLLECTION Subdivide(TRIANGLE tri, inout TriangleStream<GS_DATA> triStream)
{
	float3 m0 = (tri.position0 + tri.position1) * 0.5;
	float3 m1 = (tri.position1 + tri.position2) * 0.5;
	float3 m2 = (tri.position2 + tri.position0) * 0.5;
	
	float3 subnormal0 = (tri.normal0 + tri.normal1) / 2;
	float3 subnormal1 = (tri.normal1 + tri.normal2) / 2;
	float3 subnormal2 = (tri.normal0 + tri.normal2) / 2;
	
	float3 v[6];
	v[0] = tri.position0;
	v[1] = m0;
	v[2] = m2;
	v[3] = m1;
	v[4] = tri.position2;
	v[5] = tri.position1;
	
	triStream.RestartStrip();
	
	for (int i = 0; i < 5; ++i)
	{
		CreateVertex(triStream, v[i], subnormal0, float2(0, 0));
	}
	
	triStream.RestartStrip();
	
	CreateVertex(triStream, v[1], subnormal0, float2(0, 0));
	CreateVertex(triStream, v[5], subnormal0, float2(0, 0));
	CreateVertex(triStream, v[3], subnormal0, float2(0, 0));
	
	//new triangle top
	TRIANGLE newTriTop;
	newTriTop.position0 = v[1];
	newTriTop.position1 = v[5];
	newTriTop.position2 = v[3];
	
	newTriTop.normal0 = subnormal0;
	newTriTop.normal1 = subnormal0;
	newTriTop.normal2 = subnormal0;
		
	//new triangle left
	TRIANGLE newTriLeft;
	newTriLeft.position0 = tri.position0;
	newTriLeft.position1 = m0;
	newTriLeft.position2 = m2;
	
	newTriLeft.normal0 = subnormal0;
	newTriLeft.normal1 = subnormal0;
	newTriLeft.normal2 = subnormal0;
	
	//new triangle right
	TRIANGLE newTriRight;
	newTriRight.position0 = m2;
	newTriRight.position1 = m1;
	newTriRight.position2 = tri.position2;
	
	newTriRight.normal0 = subnormal0;
	newTriRight.normal1 = subnormal0;
	newTriRight.normal2 = subnormal0;
	
	//
	TRIANGLECOLLECTION triangleCollection;
	triangleCollection.triangle0 = newTriTop;
	triangleCollection.triangle1 = newTriLeft;
	triangleCollection.triangle2 = newTriRight;
	
	return triangleCollection;
}

[maxvertexcount(60)]
void MainGS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_DATA> triStream)
{		
	//sample height, direction and weight noise maps
	float samplePoint = 1.f;
	float samplePoint2 = 1.f;
	float samplePoint3 = 1.f;
	if (gUseHeightMap)
	{
		samplePoint = gHeightMap.SampleLevel(samgHeightMap, input[0].TexCoord, 1.0f).r;
		samplePoint2 = gHeightMap.SampleLevel(samgHeightMap, input[1].TexCoord, 1.0f).r;
		samplePoint3 = gHeightMap.SampleLevel(samgHeightMap, input[2].TexCoord, 1.0f).r;
	}

	float directionSamplePoint = 1.f;
	float directionSamplePoint2 = 1.f;
	float directionSamplePoint3 = 1.f;
	if (gUseDirectionTexture)
	{
		directionSamplePoint = gDirectionTexture.SampleLevel(samgHeightMap, input[0].TexCoord, 1.0f).r;
		directionSamplePoint2 = gDirectionTexture.SampleLevel(samgHeightMap, input[1].TexCoord, 1.0f).r;
		directionSamplePoint3 = gDirectionTexture.SampleLevel(samgHeightMap, input[2].TexCoord, 1.0f).r;
	}
		
	float weightSamplePoint = 1.f;
	float weightSamplePoint2 = 1.f;
	float weightSamplePoint3 = 1.f;
	if (gUseDirectionTexture)
	{
		weightSamplePoint = gWeightMap.SampleLevel(samgHeightMap, input[0].TexCoord, 1.0f).r;
		weightSamplePoint2 = gWeightMap.SampleLevel(samgHeightMap, input[1].TexCoord, 1.0f).r;
		weightSamplePoint3 = gWeightMap.SampleLevel(samgHeightMap, input[2].TexCoord, 1.0f).r;
	}
	
	//split the received triangle in 3 sub-triangles
	if (IsDense)
	{
		float3 m0 = (input[0].Position + input[1].Position) * 0.5;
		float3 m1 = (input[1].Position + input[2].Position) * 0.5;
		float3 m2 = (input[2].Position + input[0].Position) * 0.5;
		
		
		CreateGrass(triStream, samplePoint, directionSamplePoint, m1, input[1].Position.xyz, m0, input[0].Normal, input[1].Normal, input[2].Normal, weightSamplePoint, weightSamplePoint2, weightSamplePoint3);
		CreateGrass(triStream, samplePoint2, directionSamplePoint2, input[0].Position.xyz, m0, m2, input[0].Normal, input[1].Normal, input[2].Normal, weightSamplePoint, weightSamplePoint2, weightSamplePoint3);
		CreateGrass(triStream, samplePoint3, directionSamplePoint3, m2, m1, input[2].Position.xyz, input[0].Normal, input[1].Normal, input[2].Normal, weightSamplePoint, weightSamplePoint2, weightSamplePoint3);
	}
	else
	{
		CreateGrass(triStream, samplePoint, directionSamplePoint, input[0].Position.xyz, input[1].Position.xyz, input[2].Position.xyz, input[0].Normal, input[1].Normal, input[2].Normal, weightSamplePoint, weightSamplePoint2, weightSamplePoint3);
	}
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
	float4 finalColor = float4(0, 0, 0, 0); //simple green color
	if (gUseAddedColorTexture)
	{
		finalColor.rgb += gAddColorMap.Sample(m_TextureSampler, -input.TexCoord).rgb / 2;
	}
	
	if (gUseDiffuseTexture)
	{
		finalColor += gGrassTexture.Sample(m_TextureSampler, input.TexCoord);
		return finalColor;
		
	}

	return float4(76, 115, 49, 1);
}


technique10 DefaultTechnique
{
	pass p0
	{
		SetRasterizerState(DisableCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}