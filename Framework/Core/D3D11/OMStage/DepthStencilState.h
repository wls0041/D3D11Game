#pragma once

class DepthStencilState final : public D3D11_Object
{
public:
	DepthStencilState(class Context *context);
	~DepthStencilState();

	auto GetResource() const -> ID3D11DepthStencilState* { return state; }

	void Create
	(
		const bool& is_depth_enable,
		const D3D11_COMPARISON_FUNC& comparison = D3D11_COMPARISON_LESS_EQUAL
	);
	void Clear();

private:
	class Graphics *graphics;
	ID3D11DepthStencilState *state;
};