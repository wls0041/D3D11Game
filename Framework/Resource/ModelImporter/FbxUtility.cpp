#include "Framework.h"
#include "FbxUtility.h"
#include "FbxType.h"

AxisType FbxUtility::Axis_type = AxisType::DirectX;
const D3DXMATRIX FbxUtility::Convert_matrix;

auto FbxUtility::ToFloat(const FbxDouble & value) -> const float
{
    return static_cast<float>(value);
}

auto FbxUtility::ToVector2(const FbxVector2 & value) -> const D3DXVECTOR2
{
    return D3DXVECTOR2
    (
        ToFloat(value.mData[0]),
        ToFloat(value.mData[1])
    );
}

auto FbxUtility::ToVector3(const FbxVector4 & value) -> const D3DXVECTOR3
{
    return D3DXVECTOR3
    (
        ToFloat(value.mData[0]),
        ToFloat(value.mData[1]),
        ToFloat(value.mData[2])
    );
}

auto FbxUtility::ToVector4(const FbxVector4 & value) -> const D3DXVECTOR4
{
    return D3DXVECTOR4
    (
        ToFloat(value.mData[0]),
        ToFloat(value.mData[1]),
        ToFloat(value.mData[2]),
        ToFloat(value.mData[3])
    );
}

auto FbxUtility::ToColor(const FbxVector4 & value) -> const D3DXCOLOR
{
    return D3DXCOLOR
    (
        ToFloat(value.mData[0]),
        ToFloat(value.mData[1]),
        ToFloat(value.mData[2]),
        ToFloat(value.mData[3])
    );
}

auto FbxUtility::ToQuaternion(const FbxQuaternion & value) -> const D3DXQUATERNION
{
    return D3DXQUATERNION
    (
        ToFloat(value.mData[0]),
        ToFloat(value.mData[1]),
        ToFloat(value.mData[2]),
        ToFloat(value.mData[3])
    );
}

auto FbxUtility::ToMatrix(const FbxAMatrix & value) -> const D3DXMATRIX
{
    auto s = ToVector3(value.GetS());
    auto r = ToQuaternion(value.GetQ());
    auto t = ToVector3(value.GetT());

    D3DXMATRIX S, R, T;
    D3DXMatrixScaling(&S, s.x, s.y, s.z);
    D3DXMatrixRotationQuaternion(&R, &r);
    D3DXMatrixTranslation(&T, t.x, t.y, t.z);

    return S * R * T;
}

auto FbxUtility::ToUV(const FbxVector2 & value) -> const D3DXVECTOR2
{
	auto uv = ToVector2(value);

	switch (Axis_type)
	{
	case AxisType::Maya_Y_Up:
		return D3DXVECTOR2(uv.x, 1.0f - uv.y); //그림이 뒤집어져 있기 때문에 다시 뒤집어줌
	}

	return uv;
}

auto FbxUtility::ToVertexIndices(const std::vector<struct FbxVertexWeight>& vertex_weight) -> const D3DXVECTOR4
{
	D3DXVECTOR4 indices(0, 0, 0, 0);

	for (uint i = 0; i < vertex_weight.size(); i++) {
		switch (i)
		{
		case 0: indices.x = static_cast<float>(vertex_weight[i].index); break; 
		case 1:	indices.y = static_cast<float>(vertex_weight[i].index); break;
		case 2:	indices.z = static_cast<float>(vertex_weight[i].index); break;
		case 3:	indices.w = static_cast<float>(vertex_weight[i].index); break;
		
		default: return indices;
		}
	}
	return indices;
}

auto FbxUtility::ToVertexWeights(const std::vector<struct FbxVertexWeight>& vertex_weight) -> const D3DXVECTOR4
{
	D3DXVECTOR4 weights(0, 0, 0, 0);

	for (uint i = 0; i < vertex_weight.size(); i++) {
		switch (i)
		{
		case 0: weights.x = static_cast<float>(vertex_weight[i].weight); break;
		case 1:	weights.y = static_cast<float>(vertex_weight[i].weight); break;
		case 2:	weights.z = static_cast<float>(vertex_weight[i].weight); break;
		case 3:	weights.w = static_cast<float>(vertex_weight[i].weight); break;
		
		default: return weights;
		}
	}
	return weights;
}
