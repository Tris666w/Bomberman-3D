#include "stdafx.h"
#include "MeshFilterLoader.h"
#include "BinaryReader.h"
#include "EffectHelper.h"

//#define MULTI_TEXCOORD
#define OVM_vMAJOR 1
#ifdef MULTI_TEXCOORD
	#define OVM_vMINOR 61
#else
	#define OVM_vMINOR 1
#endif


MeshFilter* MeshFilterLoader::LoadContent(const std::wstring& assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if(!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = binReader->Read<char>();
	const int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		delete binReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(binReader->Read<char>());
		if(meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = binReader->Read<unsigned int>();

		switch(meshDataType)
		{
		case MeshDataType::HEADER:
			{
				pMesh->m_MeshName = binReader->ReadString();
				vertexCount = binReader->Read<unsigned int>();
				indexCount = binReader->Read<unsigned int>();

				pMesh->m_VertexCount = vertexCount;
				pMesh->m_IndexCount = indexCount;
			}
			break;
		case MeshDataType::POSITIONS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::POSITION);

				for(unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 pos;
					pos.x = binReader->Read<float>();
					pos.y = binReader->Read<float>();
					pos.z = binReader->Read<float>();
					pMesh->m_Positions.push_back(pos);
				}
			}
			break;
		case MeshDataType::INDICES:
			{
				for(unsigned int i = 0; i<indexCount; ++i)
				{
					pMesh->m_Indices.push_back(binReader->Read<DWORD>());
				}
			}
			break;
		case MeshDataType::NORMALS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::NORMAL);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 normal;
					normal.x = binReader->Read<float>();
					normal.y = binReader->Read<float>();
					normal.z = binReader->Read<float>();
					pMesh->m_Normals.push_back(normal);
				}
			}
			break;
		case MeshDataType::TANGENTS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TANGENT);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 tangent;
					tangent.x = binReader->Read<float>();
					tangent.y = binReader->Read<float>();
					tangent.z = binReader->Read<float>();
					pMesh->m_Tangents.push_back(tangent);
				}
			}
			break;
		case MeshDataType::BINORMALS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BINORMAL);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 binormal;
					binormal.x = binReader->Read<float>();
					binormal.y = binReader->Read<float>();
					binormal.z = binReader->Read<float>();
					pMesh->m_Binormals.push_back(binormal);
				}
			}
			break;
		case MeshDataType::TEXCOORDS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TEXCOORD);

				auto amountTexCoords = 1;
#ifdef MULTI_TEXCOORD
				amountTexCoords = binReader->Read<USHORT>();
#endif
				pMesh->m_TexCoordCount = amountTexCoords;

				for (unsigned int i = 0; i<vertexCount*amountTexCoords; ++i)
				{
				 DirectX::XMFLOAT2 tc;
					tc.x = binReader->Read<float>();
					tc.y = binReader->Read<float>();
					pMesh->m_TexCoords.push_back(tc);
				}
			}
			break;
		case MeshDataType::COLORS:
			{
				pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::COLOR);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT4 color;
					color.x = binReader->Read<float>();
					color.y = binReader->Read<float>();
					color.z = binReader->Read<float>();
					color.w = binReader->Read<float>();
					pMesh->m_Colors.push_back(color);
				}
			}
			break;
		case MeshDataType::BLENDINDICES:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDINDICES);

			for (UINT index = 0; index < pMesh->m_VertexCount; index++)
			{
				 DirectX::XMFLOAT4 blendIndex;
				 blendIndex.x = binReader->Read<float>();
				 blendIndex.y = binReader->Read<float>();
				 blendIndex.z = binReader->Read<float>();
				 blendIndex.w = binReader->Read<float>();
				pMesh->m_BlendIndices.push_back(blendIndex);
			}
			
		}
		break;
		case MeshDataType::BLENDWEIGHTS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDWEIGHTS);
				for (UINT index = 0; index < pMesh->m_VertexCount; index++)
			{
				 DirectX::XMFLOAT4 blendWeights;
				 blendWeights.x = binReader->Read<float>();
				 blendWeights.y = binReader->Read<float>();
				 blendWeights.z = binReader->Read<float>();
				 blendWeights.w = binReader->Read<float>();
				pMesh->m_BlendWeights.push_back(blendWeights);
			}
		}
		break;
		case MeshDataType::ANIMATIONCLIPS:
		{
			pMesh->m_HasAnimations = true;

			UINT16 clipCount = binReader->Read<UINT16>();
			for (UINT16 index1 = 0; index1 < clipCount;++index1)
			{
				AnimationClip clip {};
				clip.Name = binReader->ReadString();
				clip.Duration = binReader->Read<float>();
				clip.TicksPerSecond = binReader->Read<float>();
				
				UINT16 keyCount = binReader->Read<UINT16>();
				
				for (UINT16 index2 = 0; index2 < keyCount	;++index2)
				{
					AnimationKey key {};
					key.Tick = binReader->Read<float>();
					
					UINT16 transformCount = binReader->Read<UINT16>();
					for (UINT16 index3 = 0; index3 < transformCount	;++index3)
					{
						DirectX::XMFLOAT4X4 transform {};
						DirectX::XMFLOAT4 row1 = {binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>()};
						DirectX::XMFLOAT4 row2 = {binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>()};
						DirectX::XMFLOAT4 row3 = {binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>()};
						DirectX::XMFLOAT4 row4 = {binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>(),binReader->Read<float>()};
						transform = DirectX::XMFLOAT4X4{row1.x,row1.y,row1.z,row1.w,
														row2.x,row2.y,row2.z,row2.w,
														row3.x,row3.y,row3.z,row3.w,
														row4.x,row4.y,row4.z,row4.w};
						key.BoneTransforms.push_back(transform);						
					}
					clip.Keys.push_back(key);			
				}
				pMesh->m_AnimationClips.push_back(clip);
				
			}
		}
		break;
		case MeshDataType::SKELETON:
		{
			pMesh->m_BoneCount = binReader->Read<UINT16>();
			auto moveAmount = dataOffset - sizeof(UINT16);
			binReader->MoveBufferPosition(moveAmount);
				
		}
		break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
