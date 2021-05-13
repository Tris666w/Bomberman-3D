#include "stdafx.h"
#include "PhysxManager.h"
#include "PhysxAllocator.h"
#include "PhysxErrorCallback.h"
#include "GameScene.h"
#include "OverlordSimulationFilterShader.h"

static const char* DEFAULT_PVD_HOST = "127.0.0.1";
static const physx::PxU32 DEFAULT_PVD_PORT = 5425;

PhysxManager::PhysxManager():
	m_pDefaultAllocator(nullptr),
	m_pDefaultErrorCallback(nullptr),
	m_pFoundation(nullptr),
	m_pProfileZoneManager(nullptr),
	m_pPhysics(nullptr),
	m_pDefaultCpuDispatcher(nullptr),
	m_pCudaContextManager(nullptr)
{
}

PhysxManager::~PhysxManager()
{
	CleanUp();
}

void PhysxManager::CleanUp()
{
	if (m_pPhysics)
	{
		m_pPhysics->release();
	}

	if (m_pCudaContextManager)
	{
		m_pCudaContextManager->release();
	}

	if (m_pProfileZoneManager)
	{
		m_pProfileZoneManager->release();
	}

	if (m_pDefaultCpuDispatcher)
	{
		m_pDefaultCpuDispatcher->release();
	}

	if (m_pFoundation)
	{
		m_pFoundation->release();
	}

	SafeDelete(m_pDefaultAllocator);
	SafeDelete(m_pDefaultErrorCallback);
}

void PhysxManager::Init(void* /*pDevice*/)
{
	//Recording memory allocations is necessary if you want to 
	//use the memory facilities in PVD effectively.  Since PVD isn't necessarily connected
	//right away, we add a mechanism that records all outstanding memory allocations and
	//forwards them to PVD when it does connect.

	//This certainly has a performance and memory profile effect and thus should be used
	//only in non-production builds.
	bool recordMemoryAllocations = false;
#if defined(DEBUG) || defined(_DEBUG)
	recordMemoryAllocations = true;
#endif

	//WIN32 Allocator Implementation
	m_pDefaultAllocator = new PhysxAllocator();
	m_pDefaultErrorCallback = new PhysxErrorCallback();

	//Create Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_pDefaultAllocator, *m_pDefaultErrorCallback);
	if (!m_pFoundation)
	{
		Logger::LogError(L"Physx: Foundation creation failed!");
		return;
	}

	//Create ProfileZoneManager
	m_pProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(m_pFoundation);
	if (!m_pProfileZoneManager)
	{
		Logger::LogError(L"Physx: Profile Zone Manager creation failed!");
		return;
	}

	//Create Main Physics Object
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), recordMemoryAllocations,
	                             m_pProfileZoneManager);
	if (!m_pPhysics)
	{
		Logger::LogError(L"Physx: Physics Object creation failed!");
	}


	//Create Default CpuDispatcher (only 1 thread)
	m_pDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);

	//Try to connect with the PVD (Todo: link profileZone)
	ToggleVisualDebuggerConnection();
	if (m_pPhysics->getPvdConnectionManager())
		m_pPhysics->getPvdConnectionManager()->addHandler(*this);
}

physx::PxScene* PhysxManager::CreateScene(GameScene* pScene) const
{
	auto sceneDesc = physx::PxSceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	sceneDesc.gpuDispatcher = m_pCudaContextManager ? m_pCudaContextManager->getGpuDispatcher() : nullptr;
	sceneDesc.filterShader = OverlordSimulationFilterShader;
	sceneDesc.userData = pScene;

	const auto physxScene = m_pPhysics->createScene(sceneDesc);
	if (!physxScene)
	{
		Logger::LogError(L"Physx: Scene creation failed!");
	}

	return physxScene;
}

bool PhysxManager::ToggleVisualDebuggerConnection() const
{
	if (!m_pPhysics || !m_pPhysics->getPvdConnectionManager()) return false;

	auto pvdConnectionManager = m_pPhysics->getPvdConnectionManager();
	if (pvdConnectionManager->isConnected())
	{
		//DISCONNECT
		pvdConnectionManager->disconnect();
		return false;
	}
	//CONNECT

	//Use these flags for a clean profile trace with minimal overhead
	physx::PxVisualDebuggerConnectionFlags connectionFlags(physx::PxVisualDebuggerExt::getAllConnectionFlags());
	//if (!mUseFullPvdConnection)
	//	theConnectionFlags = PxVisualDebuggerConnectionFlag::Profile; 

	//The normal way to connect to pvd.  PVD needs to be running at the time this function is called.
	//We don't worry about the return value because we are already registered as a listener for connections
	//and thus our onPvdConnected call will take care of setting up our basic connection state.
	physx::PxVisualDebuggerConnection* conn = physx::PxVisualDebuggerExt::createConnection(
		pvdConnectionManager, DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, 4000, connectionFlags);

	if (conn)
	{
		conn->release();
		return true;
	}

	return false;
}

void PhysxManager::onPvdConnected(physx::PxVisualDebuggerConnection&)
{
	m_pPhysics->getVisualDebugger()->setVisualizeConstraints(true);
	//m_pPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::, true);
	//m_pPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_SCENEQUERIES, true);
}

void PhysxManager::onPvdSendClassDescriptions(physx::PxVisualDebuggerConnection&)
{
}

void PhysxManager::onPvdDisconnected(physx::PxVisualDebuggerConnection&)
{
}
