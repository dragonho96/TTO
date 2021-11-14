#pragma once
#pragma warning(disable: 4005)
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <process.h>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include <memory> // for smart ptr
#include <bitset>

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DCompiler.h>
using namespace std;


#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h> // ComPtr smartpointer for COM
#include <wrl\client.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

#include "PxPhysicsAPI.h"
using namespace physx;

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
//#include "yaml-cpp\yaml.h"

using namespace Engine;


