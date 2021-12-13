#pragma once
#pragma warning (disable : 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)


#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <fstream>
#include <unordered_map>
#include <process.h>
#include <algorithm>
#include <typeinfo>
#include <memory> // for smart ptr
#include <bitset>
#include <map>

#include <d3d11.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>
#include "d3dxGlobal.h"
#include "DirectXTex.h"
#include <DirectXMath.h>

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

//#include <DirectXPackedVector.h>
//using namespace DirectX::PackedVector;


using namespace DirectX;

#include <wrl.h> // ComPtr smartpointer for COM
#include <wrl\client.h>
using namespace std;


#include "PxPhysicsAPI.h"
using namespace physx;


#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#include "imgui_stdlib.h"

#include <fmod.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <experimental\filesystem>

#define FILESYSTEM std::experimental::filesystem

// For Memory Leak
#ifndef _TOOL
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif
#endif

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"



//#include "Engine.h"
#include "yaml-cpp\yaml.h"

using namespace Engine;


