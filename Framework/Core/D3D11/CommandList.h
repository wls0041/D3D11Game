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
	CommandList(class Context* context);
	~CommandList();
	
	void Begin(const std::string& pass_name);
	void End();

	void Draw(const uint &vertex_count);
	void DrawIndexed(const uint &index_count, const uint &index_offset = 0, const uint &vertex_offset = 0);

	void SetVertexBuffer(VertexBuffer* buffer);
	void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer);

	void SetIndexBuffer(IndexBuffer* buffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer);

	void SetInputLayout(InputLayout* input_layout);
	void SetInputLayout(const std::shared_ptr<InputLayout>& input_layout);

	void SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY& primitive_topology);

	void SetVertexShader(class Shader* shader);
	void SetVertexShader(const std::shared_ptr<class Shader>& shader);
	void SetVertexShader(VertexShader* shader);
	void SetVertexShader(const std::shared_ptr<VertexShader>& shader);

	void SetPixelShader(class Shader* shader);
	void SetPixelShader(const std::shared_ptr<class Shader>& shader);
	void SetPixelShader(PixelShader* shader);
	void SetPixelShader(const std::shared_ptr<PixelShader>& shader);

	void SetConstantBuffer(const uint& slot, const ShaderScope& scope, ConstantBuffer* buffer);
	void SetConstantBuffer(const uint& slot, const ShaderScope& scope, const std::shared_ptr<ConstantBuffer>& buffer);
	void SetConstantBuffers(const uint& slot, const ShaderScope& scope, const std::vector<ID3D11Buffer*>& buffers);

	void SetShaderResource(const uint& slot, const ShaderScope& stage, Texture* texture);
	void SetShaderResource(const uint& slot, const ShaderScope& stage, const std::shared_ptr<Texture>& texture);
	void SetShaderResources(const uint& slot, const ShaderScope& stage, const std::vector<ID3D11ShaderResourceView*>& shader_resources);

	//void SetSamplerState(const uint& slot, const ShaderScope& stage, SamplerState* sampler_state);
	//void SetSamplerState(const uint& slot, const ShaderScope& stage, const std::shared_ptr<SamplerState>& sampler_state);
	void SetSamplerStates(const uint& slot, const ShaderScope& stage, const std::vector<ID3D11SamplerState*>& sampler_states);

	void SetViewport(const D3D11_VIEWPORT& viewport);

	//void SetRasterizerState(RasterizerState* rasterizer_state);
	//void SetRasterizerState(const std::shared_ptr<RasterizerState>& rasterizer_state);

	//void SetBlendState(BlendState* blend_state);
	//void SetBlendState(const std::shared_ptr<BlendState>& blend_state);

	void SetDepthStencilState(DepthStencilState* depth_stencil_state);
	void SetDepthStencilState(const std::shared_ptr<DepthStencilState>& depth_stencil_state);

	void SetRenderTarget(Texture* render_target, Texture* depth_stencil_target = nullptr);
	void SetRenderTarget(const std::shared_ptr<Texture>& render_target, const std::shared_ptr<Texture>& depth_stencil_target = nullptr);
	void SetRenderTargets(const std::vector<ID3D11RenderTargetView*>& render_targets, ID3D11DepthStencilView* depth_stencil_target = nullptr);

	void ClearRenderTarget(Texture* render_target, const Color4& clear_color = Color4::Zero);
	void ClearRenderTarget(const std::shared_ptr<Texture>& render_target, const Color4& clear_color = Color4::Zero);
	void ClearRenderTargets(const std::vector<ID3D11RenderTargetView*>& render_targets, const Color4& clear_color = Color4::Zero);

	void ClearDepthStencilTarget(Texture* depth_stencil_target, const D3D11_CLEAR_FLAG& clear_flag, const float& clear_depth = 1.0f, const uint& clear_stencil = 0U);
	void ClearDepthStencilTarget(const std::shared_ptr<Texture>& depth_stencil_target, const D3D11_CLEAR_FLAG& clear_flag, const float& clear_depth = 1.0f, const uint& clear_stencil = 0U);

	void ClearShaderResources(const ShaderScope& scope);


	auto Submit() -> const bool;

private:
	auto GetCommand()->Command&;
	void Clear();

private:
	class Graphics* graphics;
	std::vector<Command> commands;
	uint command_capacity;
	uint command_count;
};