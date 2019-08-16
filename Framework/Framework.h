#pragma once
#pragma warning(disable : 4506) //인라인 함수가 h에 정의되지 않음 오류

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
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <deque>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <any> 
#include <variant> 
#include <atomic> 

//DirectX
#include <d3dcompiler.h>
#include <d3d11_4.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//Typedef
typedef unsigned int uint;

//Macro Function
#define SAFE_RELEASE(p)         { if(p) { (p)->Release();   (p) = nullptr; } }
#define SAFE_DELETE(p)          { if(p) { delete (p);       (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p)    { if(p) { delete[] (p);     (p) = nullptr; } }

//Math
#include "./Math/Math.h"
#include "./Math/Vector2.h"
#include "./Math/Vector3.h"
#include "./Math/Vector4.h"
#include "./Math/Color4.h"
#include "./Math/Quaternion.h"
#include "./Math/Matrix.h"

//Utility
#include "./Utility/FileSystem.h"
#include "./Utility/FileStream.h"
#include "./Utility/Geometry_Generator.h"
#include "./Utility/GUID_Generator.h"

//Log
#include "./Log/Log.h"

//Main System
#include "./Core/Settings.h"
#include "./Core/Context.h"
#include "./Core/Engine.h"

//EventSystem
#include "./Core/EventSystem/EventSystem.h"

//Basic Data
#include "./Core/D3D11/BasicData/D3D11_Config.h"
#include "./Core/D3D11/BasicData/Vertex.h"
#include "./Core/D3D11/BasicData/Geometry.h"

//Subsystem
#include "./Core/Subsystem/Timer.h"
#include "./Core/Subsystem/Input.h"
#include "./Core/Subsystem/Thread.h"
#include "./Core/Subsystem/Graphics.h"
#include "./Core/Subsystem/ResourceManager.h"
#include "./Core/Subsystem/SceneManager.h"
#include "./Core/Subsystem/Renderer.h"

//DirectX pipeline wrapper
#include "./Core/D3D11/IAStage/VertexBuffer.h"
#include "./Core/D3D11/IAStage/IndexBuffer.h"
#include "./Core/D3D11/IAStage/InputLayout.h"
#include "./Core/D3D11/ShaderStage/VertexShader.h"
#include "./Core/D3D11/ShaderStage/PixelShader.h"
#include "./Core/D3D11/ShaderStage/ConstantBuffer.h"

//Shader
#include "./Shader/Shader.h"

//Resource
#include "./Resource/Texture2D.h"
#include "./Resource/Mesh.h"
#include "./Resource/Material.h"
#include "./Resource/Model.h"

