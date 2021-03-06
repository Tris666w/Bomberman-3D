#pragma once

#pragma region
//PhysX:you must suppress its definition of the macros min and max, as these are common C++ method names in the SDK
#define NOMINMAX
// Windows Header Files:
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "XINPUT9_1_0.LIB")

//C RunTime Header Files
#include <wchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <locale.h>
#include <codecvt>

#pragma endregion Windows

#pragma region
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//DirectXMath
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#pragma endregion DirectX

#pragma region
#pragma warning( push )
#pragma warning (disable: 26495 26812)
#include <PxPhysicsAPI.h>

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "PxTaskDEBUG.lib")
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")

#else 
#pragma comment(lib, "PxTask.lib")
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#endif
#pragma warning( pop )
#pragma endregion PHYSX INCLUDES

#pragma region
//**EFFECTS11 (Helper for loading Effects (D3DX11))
//https://fx11.codeplex.com/
#include "d3dx11effect.h" //[AdditionalLibraries/DX_Effects11/include/d3dx11effect.h]
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DxEffects11_vc14_Debug.lib")
#else 
#pragma comment(lib, "DxEffects11_vc14_Release.lib")
#endif

//*DXTEX (Helper for loading Textures (D3DX11))
//http://directxtex.codeplex.com/
#pragma warning( push )
#pragma warning (disable: 26495 26439 26812)
#include "DirectXTex.h"
#pragma warning (pop)
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DxTex_vc14_Debug.lib")
#else 
#pragma comment(lib, "DxTex_vc14_Release.lib")
#endif

//*FMOD (Sounds)
//www.fmod.org
#pragma warning(push)
#pragma warning(disable: 4505 26812)
#include "fmod.hpp" //Precompiled Header
#include "fmod_errors.h"
#pragma warning(pop)

#pragma comment(lib, "fmod_vc.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "fmodL_vc.lib")
#endif
#pragma endregion Third-Party Libs

#pragma region
//*****************************************************************************
//Declare templates for releasing interfaces and deleting objects 
//*****************************************************************************
template<class Interface>
inline void SafeRelease(Interface& pInterfaceToRelease)
{
	if (pInterfaceToRelease != nullptr)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = nullptr;
	}
}

template<class T>
inline void SafeDelete(T& pObjectToDelete)
{
	if (pObjectToDelete != nullptr)
	{
		delete(pObjectToDelete);
		pObjectToDelete = nullptr;
	}
}

template<typename T>
inline void Clamp(T& value, T hi, T lo)
{
	if (value > hi)
		value = hi;

	if (value < lo)
		value = lo;
}

template<typename T>
inline T Interpolate(T value1, T value2, T t)
{
	return (1.0f - t) * value1 + t * value2;
}

inline int GetArrayIndex(int const x, int const y, int const width)
{
	return y * width + x;
}
inline int GetArrayIndex(int const x, int const y, int const z, int const width, int const height)
{
	return z * width * height + y * width + x;
}

//String conversion source : https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
//Comment by user "zumalifeguard"
inline std::string WstringToString(const std::wstring& s)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(s);
}


inline std::wstring StringToWString(const std::string& s)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(s);
}
#pragma endregion Templates & Macros

#include "GeneralStructs.h"
#include "MathHelper.h"
#include "PhysXHelper.h"
#include "VertexHelper.h"
#include "Logger.h"