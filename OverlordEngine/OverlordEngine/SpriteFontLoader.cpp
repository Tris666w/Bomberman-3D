#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "TextureDataLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	BinaryReader* pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	
	char const identifier1 = pBinReader->Read<char>();
	char const identifier2 = pBinReader->Read<char>();
	char const identifier3 = pBinReader->Read<char>();

	if (identifier1 != 'B' || identifier2 != 'M' ||identifier3 != 'F')
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}
	
	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...
	char const version = pBinReader->Read<char>();

	if (version != 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}
	
	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	char blockId = pBinReader->Read<char>();
	int blockSize = pBinReader->Read<int>();
	pSpriteFont->m_FontSize = pBinReader->Read<short>();
	pBinReader->MoveBufferPosition(12);
	pSpriteFont->m_FontName = pBinReader->ReadNullString();
	


	//**********
	// BLOCK 1 *
	//**********
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	pBinReader->MoveBufferPosition(4);
	pSpriteFont->m_TextureWidth =pBinReader->Read<short>();
	pSpriteFont->m_TextureHeight =pBinReader->Read<short>();
	
	int const pageCount = pBinReader->Read<short>();
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
		return nullptr;
	}
	pBinReader->MoveBufferPosition(5);
	
	
	//**********
	// BLOCK 2 *
	//**********
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	std::wstring const pageName = pBinReader->ReadNullString();
	if (pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
		return nullptr;
	}
	size_t const filepathEndPos = assetFile.rfind(pSpriteFont->m_FontName);
	std::wstring const path = assetFile.substr(0,filepathEndPos);
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(path + pageName);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	
	int const charCount = (int)blockSize / sizeof(FontMetric);
	for (int index = 0; index<charCount;++index)
	{
		auto const characterId = static_cast<wchar_t>(pBinReader->Read<UINT32>());
		if(!pSpriteFont->IsCharValid(characterId))
		{
			Logger::LogWarning(characterId + L" is not valid");
			continue;
		}
		auto& fontMetric = pSpriteFont->GetMetric(characterId);
		fontMetric.IsValid = true;
		fontMetric.Character = characterId;
		float const xPos = pBinReader->Read<UINT16>();
		float const yPos = pBinReader->Read<UINT16>();
		fontMetric.Width = pBinReader->Read<UINT16>();
		fontMetric.Height = pBinReader->Read<UINT16>();
		fontMetric.OffsetX = pBinReader->Read<int16_t>();
		fontMetric.OffsetY = pBinReader->Read<int16_t>();
		fontMetric.AdvanceX = pBinReader->Read<int16_t>();
		fontMetric.Page = pBinReader->Read<char>();
		int const channel = pBinReader->Read<UINT8>();
		switch (channel)
		{
		case 1:
			fontMetric.Channel = 2;
			break;
		case 2:
			fontMetric.Channel = 1;
			break;
		case 4:
			fontMetric.Channel = 0;
			break;
		case 8:
			fontMetric.Channel = 3;
			break;
		}
		fontMetric.TexCoord.x = xPos / pSpriteFont->m_TextureWidth;
		fontMetric.TexCoord.y = yPos / pSpriteFont->m_TextureHeight;
		
	}
	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
