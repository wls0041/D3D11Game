#pragma once

#define NOMINMAX 

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#endif


//Window
#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>

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

//FBX SDK
#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

//Utility
#include "./Utility/FileSystem.h"
#include "./Utility/Geometry_Generator.h"

//Log
#include "./Log/Log.h"

//Main System
#include "./Core/Settings.h"
#include "./Core/Context.h"
#include "./Core/Engine.h"

//Basic Data
#include "./Core/D3D11/BasicData/D3D11_Config.h"
#include "./Core/D3D11/BasicData/Vertex.h"
#include "./Core/D3D11/BasicData/Geometry.h"

//Subsystem
#include "./Core/Subsystem/Timer.h"
#include "./Core/Subsystem/Graphics.h"
#include "./Core/Subsystem/Input.h"

//DirectX pipeline wrapper
#include "./Core/D3D11/IAStage/VertexBuffer.h"
#include "./Core/D3D11/IAStage/IndexBuffer.h"
#include "./Core/D3D11/IAStage/InputLayout.h"
#include "./Core/D3D11/ShaderStage/VertexShader.h"
#include "./Core/D3D11/ShaderStage/PixelShader.h"
#include "./Core/D3D11/ShaderStage/ConstantBuffer.h"

//Resource
#include "./Resource/Model.h"