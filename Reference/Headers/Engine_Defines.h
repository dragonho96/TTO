#pragma once

#include <iostream>
#include <fstream>
#include <d3d11.h>
#include <d3dx11.h>
#include <process.h>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr smartpointer for COM
#include <wrl\client.h>

#include <memory> // for smart ptr

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


// For Memory Leak
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif




#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

#include "yaml-cpp\yaml.h"


using namespace Engine;


