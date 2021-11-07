#pragma once

#include <iostream>
#include <fstream>
#include <d3d11.h>
#include <d3dx11.h>
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

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

#include "yaml-cpp\yaml.h"


using namespace Engine;


