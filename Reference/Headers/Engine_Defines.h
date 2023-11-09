#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX; //! XMFLOAT등등의 자료형을 사용하기위함.

#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
using namespace std;

#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif



