#include "stdafx.h"
#include "BombermanGameSettings.h"

float BombermanGameSettings::breakable_wall_spawn_percentage = 0.5f;
float BombermanGameSettings::sound_effect_volume = 0.f;
float BombermanGameSettings::music_volume = 0.f;

int BombermanGameSettings::grid_size = 11;
int BombermanGameSettings::wall_height = 2;
int BombermanGameSettings::block_size = 6;

std::wstring BombermanGameSettings::player_tag = L"PlayerChar";
std::wstring BombermanGameSettings::bomb_tag = L"Bomb";
std::wstring BombermanGameSettings::destructible_tag = L"Destructible";