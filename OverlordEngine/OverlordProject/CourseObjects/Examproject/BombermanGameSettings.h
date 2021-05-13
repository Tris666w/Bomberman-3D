#pragma once
struct BombermanGameSettings final
{
	static int block_size;
	static int grid_size;
	static int wall_height;
	//Should be float between [0;1]
	static float breakable_wall_spawn_percentage;

	//Volume of the sound effects
	static float sound_effect_volume;
	//Volume of the music
	static float music_volume;

	static std::wstring player_tag;
	static std::wstring bomb_tag;
	static std::wstring destructible_tag;

};

