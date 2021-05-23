#pragma once
#include <GameScene.h>
class BombermanCharPrefab;

class BombermanScene final :public GameScene
{
public:
	BombermanScene();
	~BombermanScene() override;

	/// @brief Check if there is only 1 player remaining, if so finish the game
	void CheckForGameEnd();
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;
private:
	CameraComponent* m_pCamera = nullptr;
	int m_MenuActionID = -1;
	FMOD::Channel* m_pAmbientSoundChannel = nullptr;
	std::vector<BombermanCharPrefab*>m_CharPrefabs;

	/// @brief Creates the level
	void CreateLevel();

	/// @brief Create grass
	void CreateVegetation();

	/// @brief Creates the floor plane
	void CreateFloor();

	/// @brief Create the walls of the floor
	/// @param size the size of the grid with width: size and height: size
	/// @param wallHeight height of the walls in units (will be multiplied with blockSize)
	void CreateWalls(int const size);

	/// @brief creates and fills the grid with stumps and static walls
	/// @param size size of the grid
	void CreateAndFillGrid(int const size);

	///@brief Creates the sky box
	void CreateSkybox();

	/// @brief Creates the fixed camera
	/// @param pos the position of the camera
	/// @param rot the rotation of the camera
	void CreateAndActivateCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot);

	/// @brief Initialize the players and UI
	void CreatePlayers();

	/// @brief Initialize powerUps according to the bomberman game settings
	void CreatePowerUps();
	
	/// @brief Creates the camp site
	void CreateCampSite();

	/// @brief Loads the in-game men8
	void LoadInGameMenu()const;

	/// @brief Finish the game
	/// @param playerIndex index of the winner
	void FinishGame(size_t playerIndex);

	/// @brief initializes the ambient music
	void InitSound();

	void InitPostProcessFilters();
};

