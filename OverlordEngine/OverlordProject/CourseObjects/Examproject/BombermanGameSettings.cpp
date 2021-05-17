#include "stdafx.h"
#include "BombermanGameSettings.h"

BombermanGameSettings::~BombermanGameSettings()
{	
}

int BombermanGameSettings::GetAmountOfPlayers() const
{
	return m_AmountOfPLayers;
}

int BombermanGameSettings::GetBlockSize() const
{
	return m_BlockSize;
}

int BombermanGameSettings::GetGridSize() const
{
	return m_GridSize;
}

int BombermanGameSettings::GetWallHeight() const
{
	return m_WallHeight;
}

float BombermanGameSettings::GetBreakableSpawnPercent() const
{
	return m_BreakableSpawnPercentage;
}

float BombermanGameSettings::GetSoundVolume() const
{
	return m_SoundVolume;
}

float BombermanGameSettings::GetMusicVolume() const
{
	return m_MusicVolume;
}

std::wstring BombermanGameSettings::GetPlayerTag() const
{
	return m_PlayerTag;
}

std::wstring BombermanGameSettings::GetDestructibleTag() const
{
	return m_DestructibleTag;
}

std::wstring BombermanGameSettings::GetBombTag() const
{
	return m_BombTag;
}

const std::vector<std::vector<int>>& BombermanGameSettings::GetControlsVector() const
{
	return m_ControlsVector;
}

void BombermanGameSettings::SetSoundVolume(float volume)
{
	m_SoundVolume = volume;
}

void BombermanGameSettings::SetMusicVolume(float volume)
{
	m_MusicVolume = volume;
}

BombermanGameSettings::BombermanGameSettings()
{
	m_ControlsVector.push_back({VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT});
	m_ControlsVector.push_back({'A','D','W','S','X'});
	m_ControlsVector.push_back({VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT});
	m_ControlsVector.push_back({VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT});
}

