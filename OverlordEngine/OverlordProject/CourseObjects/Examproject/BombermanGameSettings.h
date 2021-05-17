#pragma once
#include <Singleton.h>

class BombermanGameSettings final: public Singleton<BombermanGameSettings>
{
public:
	~BombermanGameSettings();
	
	//Getters
	
	[[nodiscard]]int GetAmountOfPlayers()const;
	[[nodiscard]]int GetBlockSize()const;
	[[nodiscard]]int GetGridSize()const;
	[[nodiscard]]int GetWallHeight()const;
	
	[[nodiscard]]float GetBreakableSpawnPercent()const;
	[[nodiscard]]float GetSoundVolume()const;
	[[nodiscard]]float GetMusicVolume()const;
  
	[[nodiscard]]std::wstring GetPlayerTag()const;
	[[nodiscard]]std::wstring GetDestructibleTag()const;
	[[nodiscard]]std::wstring GetBombTag()const;

	[[nodiscard]]const std::vector<std::vector<int>>& GetControlsVector()const;
	
	//Setters
	
	

private:
	friend class Singleton<BombermanGameSettings>;

	BombermanGameSettings();

	int m_AmountOfPLayers = 4;
	int m_BlockSize = 6;
	int m_GridSize = 11;
	int m_WallHeight = 2;
	//Should be float between [0;1]
	float m_BreakableSpawnPercentage = 0.5f;

	//Volume of the sound effects
	float m_SoundVolume = 0.f;
	//Volume of the music
	float m_MusicVolume = 0.f;

	std::wstring m_PlayerTag = L"PlayerChar";
	std::wstring m_BombTag =  L"Bomb";
	std::wstring m_DestructibleTag = L"Destructible";

	std::vector<std::vector<int>> m_ControlsVector{};
};

