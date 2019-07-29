#pragma once

enum class CommandType : uint
{
	Unknown,
	Begin,
	End,
	Draw,
	DrawIndexed,
	SetVertexBuffer,
	SetIndexBuffer,
	SetInputLayout,
	SetPrimitiveTopology,
	SetVertexShader,
	SetPixelShader,
	SetHullShader,
	SetDomainShader,
	SetGeometryShader,
	SetComputeShader,
	SetConstantBuffer,
	SetShaderResource,
	SetSamplerState,
	SetRasterizerState,
	SetViewport,
	SetBlendState,
	SetDepthStencilState,
	SetRenderTarget,
	ClearRenderTarget,
	ClearDepthStencilTarget,
};
struct Command final
{
	Command();
	void Clear();

	CommandType command_type;
	std::string pass_name;

	ID3D11Buffer* vertex_buffer;
	uint vertex_count;
	uint vertex_offset;
	uint vertex_stride;

	ID3D11Buffer* index_buffer;
	uint index_count;
	uint index_offset;

	ID3D11InputLayout* input_layout;
	D3D11_PRIMITIVE_TOPOLOGY primitive_topology;

	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11HullShader* hull_shader;
	ID3D11DomainShader* domain_shader;
	ID3D11GeometryShader* geometry_shader;
	ID3D11ComputeShader* compute_shader;

	std::vector<ID3D11Buffer*> constant_buffers;
	ShaderScope constant_buffer_shader_scope;
	uint constant_buffer_count;
	uint constant_buffer_slot;

	std::vector<ID3D11ShaderResourceView*> shader_resources;
	ShaderScope shader_resource_shader_scope;
	uint shader_resource_count;
	uint shader_resource_slot;

	std::vector<ID3D11SamplerState*> samplers;
	ShaderScope sampler_shader_scope;
	uint sampler_count;
	uint sampler_slot;

	ID3D11RasterizerState* rasterizer_state;
	D3D11_VIEWPORT viewport;

	ID3D11BlendState* blend_state;
	ID3D11DepthStencilState* depth_stencil_state;

	std::vector<ID3D11RenderTargetView*> render_targets;
	uint render_target_count;

	ID3D11DepthStencilView* depth_stencil_target;

	ID3D11RenderTargetView* clear_render_target;
	Color4 clear_color;

	ID3D11DepthStencilView* clear_depth_stencil_target;
	uint clear_flags;
	uint clear_stencil;
	float clear_depth;
};

class CommandList final
{
public:
	CommandList(class Context *context);
	~CommandList();

private:
	auto GetCommand() -> Command&;
	void Clear();
private:
	class Graphics *graphics;
	std::vector<Command> commands;
	uint command_capacity;
	uint command_count;
};