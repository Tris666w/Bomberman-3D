#pragma once
#include <GameScene.h>

class BombermanScene final :public GameScene
{
public:
	BombermanScene();
	~BombermanScene() override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	CameraComponent* m_pCamera = nullptr;

	/// @brief prints the character controls
	void PrintControls()const;
	/// @brief Creates the floor grid
	/// @param size The size of the grid with width: size and height: size
	void CreateFloor(int const size);

	/// @brief Create the walls of the floor
	/// @param size the size of the grid with width: size and height: size
	/// @param wallHeight heighht of the walls in units (will be multiplied with blockSize)
	void CreateWalls(int const size, int const wallHeight);

	/// @brief Create the static walls in the grid
	/// @param size size of the grid
	void CreateNotDestructibleWalls(int const size);

	/// @brief Create the explode-able walls
	/// @param size size of the grid
	void CreateDestructibleWalls(int const size);

	///@brief Creates the sky box
	void CreateSkybox();

	/// @brief Creates the fixed camera
	/// @param pos the position of the camera
	/// @param rot the rotation of the camera
	void CreateAndActivateCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot);

	void CreatePlayers();
};

