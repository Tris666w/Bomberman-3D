#include "stdafx.h"
#include "BombermanGameSettings.h"

float BombermanGameSettings::breakable_wall_spawn_percentage = 0.75f;
int BombermanGameSettings::grid_size = 11;

std::wstring BombermanGameSettings::player_tag = L"PlayerChar";
std::wstring BombermanGameSettings::bomb_tag = L"Bomb";
std::wstring BombermanGameSettings::destructible_tag = L"Destructible";