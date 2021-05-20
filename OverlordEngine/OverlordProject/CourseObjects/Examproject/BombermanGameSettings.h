#pragma once
#include <Singleton.h>
enum class InputScheme
{
	Keyboard1 = 0,
	Keyboard2 = 1,
	Controller = 2,

	Count = 3
};

class BombermanGameSettings final : public Singleton<BombermanGameSettings>
{
public:
	~BombermanGameSettings();

	//Getters
	[[nodiscard]] int GetAmountOfPlayers()const;
	[[nodiscard]] int GetBlockSize()const;
	[[nodiscard]] int GetGridSize()const;
	[[nodiscard]] int GetWallHeight()const;

	[[nodiscard]] float GetBreakableSpawnPercent()const;
	[[nodiscard]] float GetSoundVolume()const;
	[[nodiscard]] float GetMusicVolume()const;

	[[nodiscard]] std::wstring GetPlayerTag()const;
	[[nodiscard]] std::wstring GetDestructibleTag()const;
	[[nodiscard]] std::wstring GetBombTag()const;

	using SchemePair = std::pair<std::vector<int>, std::wstring>;
	[[nodiscard]] const std::unordered_map<InputScheme, SchemePair>& GetSchemeMap()const;
	[[nodiscard]] InputScheme GetPlayerInputScheme(int playerIndex);

	//Setters
	void SetSoundVolume(float volume);
	void SetMusicVolume(float volume);
	void SetAmountOfPlayers(int amountOfPlayers);
	void SetPlayerInput(int playerIndex, InputScheme newScheme);

	static int const max_amount_of_players = 4;
	static int const min_amount_of_players = 2;
	inline static DirectX::XMFLOAT4 const text_color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Gold);
	inline static DirectX::XMFLOAT4 const button_color_passive = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Goldenrod);
	inline static DirectX::XMFLOAT4 const button_color_hover = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkGoldenrod);

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
	float m_SoundVolume = 0.9f;
	//Volume of the music
	float m_MusicVolume = 0.2f;

	std::wstring m_PlayerTag = L"PlayerChar";
	std::wstring m_BombTag = L"Bomb";
	std::wstring m_DestructibleTag = L"Destructible";

	std::unordered_map<InputScheme, SchemePair>m_SchemeMap{};
	std::vector<InputScheme> m_PlayerSchemes{};
};

