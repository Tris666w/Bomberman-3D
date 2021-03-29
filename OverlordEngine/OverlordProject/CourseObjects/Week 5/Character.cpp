
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"


Character::Character(float radius, float height, float moveSpeed) : 
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f), 
	m_TerminalVelocity(20),
	m_Gravity(9.81f), 
	m_RunAccelerationTime(0.3f), 
	m_JumpAccelerationTime(0.8f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 
	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0)
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Create controller

	//TODO: Add a fixed camera as child

	//TODO: Register all Input Actions
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
}

void Character::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: Update the character (Camera rotation, Character Movement, Character Gravity)
}