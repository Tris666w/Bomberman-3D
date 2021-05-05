#pragma once
struct BombermanGameSettings final
{
	static int const block_size = 6;
	static int grid_size;
	static int const wall_height = 2;
	//Should be float between [0;1]
	static float breakable_wall_spawn_percentage;
	static std::wstring player_tag;
	static std::wstring bomb_tag;
	static std::wstring destructible_tag;

};

