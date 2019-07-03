#pragma once

enum class AxisType : uint
{
    Maya_Y_Up,
    DirectX
};

class FbxUtility final
{
public:
    static AxisType Axis_type;
    static const D3DXMATRIX Convert_matrix;

public:
    static auto ToFloat(const FbxDouble& value) -> const float;
    static auto ToVector2(const FbxVector2& value) -> const D3DXVECTOR2;
    static auto ToVector3(const FbxVector4& value) -> const D3DXVECTOR3;
    static auto ToVector4(const FbxVector4& value) -> const D3DXVECTOR4;
    static auto ToColor(const FbxVector4& value) -> const D3DXCOLOR;
    static auto ToQuaternion(const FbxQuaternion& value) -> const D3DXQUATERNION;
    static auto ToMatrix(const FbxAMatrix& value) -> const D3DXMATRIX;

	static auto ToUV(const FbxVector2 &value) -> const D3DXVECTOR2;
};