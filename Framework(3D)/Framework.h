#pragma once

//Window
#include <Windows.h>
#include <assert.h>

//STL
#include <vector>

//DirectX
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//Typedef
typedef unsigned int uint;

//Macro Function
#define SAFE_RELEASE(p)         { if(p) { (p)->Release();   (p) = nullptr; } }
#define SAFE_DELETE(p)          { if(p) { delete (p);       (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p)    { if(p) { delete[] (p);     (p) = nullptr; } }

//
#include "./Core/Settings.h"