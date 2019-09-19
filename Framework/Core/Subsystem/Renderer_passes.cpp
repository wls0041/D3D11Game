#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Actor.h"
#include "../../Scene/Component/Camera.h"
#include "../../Scene/Component/Transform.h"
#include "../../Scene/Component/Renderable.h"
#include "Gizmo/Grid.h"

void Renderer::PassMain()
{
	command_list->Begin("PassMain");
	{
		PassGBuffer();        
		PassLine(render_textures[RenderTargetType::Final]);
		PassDebug(render_textures[RenderTargetType::Final]);
	}
	command_list->End();
	command_list->Submit();
}

void Renderer::PassGBuffer()
{
	command_list->Begin("PassGBuffer");
	{
        auto& albedo_texture    = render_textures[RenderTargetType::GBuffer_Albedo];//한계 8개
        auto& normal_texture    = render_textures[RenderTargetType::GBuffer_Normal];
        auto& material_texture  = render_textures[RenderTargetType::GBuffer_Material];
        auto& velocity_texture  = render_textures[RenderTargetType::GBuffer_Velocity];
        auto& depth_texture     = render_textures[RenderTargetType::GBuffer_Depth];//한계 1개

		if (renderables[RenderableType::Opaque].empty()) { //투명 객체
			command_list->ClearRenderTarget(albedo_texture);
			command_list->ClearRenderTarget(normal_texture);
			command_list->ClearRenderTarget(material_texture);
			command_list->ClearRenderTarget(velocity_texture);
			command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH, GetClearDepth()); //depth는 렌더타겟이 없음. 1.0 - 추후 값 수정할 것임. z축 바뀔 때 뒤집힘
			command_list->End();
			command_list->Submit();
			return;
		}

		const auto &vertex_shader = shaders[ShaderType::VS_GBUFFER];
		if (!vertex_shader) return;

		std::vector<ID3D11RenderTargetView*> render_targets{

			albedo_texture->GetRenderTargetView(),
			normal_texture->GetRenderTargetView(),
			material_texture->GetRenderTargetView(),
			velocity_texture->GetRenderTargetView()
		};

		UpdateGlobalBuffer(static_cast<uint>(resolution.x), static_cast<uint>(resolution.y));

		uint current_material_id = 0;
		uint current_mesh_id = 0;
		uint current_shader_id = 0;

		const auto draw_actor = [this, &current_material_id, &current_mesh_id, &current_shader_id](Actor *actor) {
			const auto &renderable = actor->GetRenderable();
			if (!renderable) return;

			const auto &material = renderable->GetMaterial();
			if (!material) return;

			const auto &shader = material->GetShader();
			if (!shader) return;
			
			const auto &mesh= renderable->GetMesh();
			if (!mesh || !mesh->GetVertexBuffer() || !mesh->GetIndexBuffer()) return;
			
			if (current_mesh_id != mesh->GetResourceID()) {
				command_list->SetVertexBuffer(mesh->GetVertexBuffer());
				command_list->SetIndexBuffer(mesh->GetIndexBuffer());
				current_mesh_id = mesh->GetResourceID();
			}

			if (current_shader_id != shader->GetShaderID()) {
				command_list->SetPixelShader(shader);
				current_shader_id = shader->GetShaderID();
			}

			if (current_material_id != material->GetResourceID()) {
				command_list->SetRasterizerState(GetRasterizerState(material->GetCullMode()));
				command_list->SetShaderResources(0, ShaderScope::PS, material->GetTextureShaderResources());
				
				material->UpdateConstantBuffer(); //Material에 만들어 놓은 roughtness, metallic 등의 요소 업데이트
				command_list->SetConstantBuffer(1, ShaderScope::PS, material->GetConstantBuffer());
				current_material_id = material->GetResourceID();
			}

			const auto &transform = actor->GetTransform();
			transform->UpdateConstantBuffer(camera_view_proj); //Gbuffer.hlsl의 material_buffer, model_buffer 업데이트
			command_list->SetConstantBuffer(2, ShaderScope::VS, transform->GetConstantBuffer());

			command_list->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), mesh->GetIndexBuffer()->GetOffset(), mesh->GetVertexBuffer()->GetOffset());
		};

		command_list->SetBlendState(blend_enabled);
		command_list->SetDepthStencilState(depth_stencil_enabled_state);
		command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command_list->SetRenderTargets(render_targets, depth_texture->GetDepthStencilView());
		command_list->SetViewport(albedo_texture->GetViewport());
		command_list->ClearRenderTargets(render_targets);
		command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH, GetClearDepth());
		command_list->SetVertexShader(vertex_shader);
		command_list->SetInputLayout(vertex_shader->GetInputLayout());
		command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
		command_list->SetSamplerState(0, ShaderScope::PS, anisotropic_wrap);

		for (const auto &actor : renderables[RenderableType::Opaque])
			draw_actor(actor);

		command_list->SetBlendState(blend_color_add);
		for (const auto &actor : renderables[RenderableType::Transparent])
			draw_actor(actor);
	}
	command_list->End();
	command_list->Submit();
}

void Renderer::PassLine(std::shared_ptr<class Texture>& out)
{
	const auto& shader = shaders[ShaderType::VPS_COLOR];
	if (!shader)
		return;

	command_list->Begin("PassLine");

	command_list->SetRasterizerState(cull_back_wireframe);
	command_list->SetBlendState(blend_disabled);
	command_list->SetViewport(out->GetViewport());
	command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	command_list->SetVertexShader(shader);
	command_list->SetPixelShader(shader);
	command_list->SetInputLayout(shader->GetInputLayout());
	command_list->SetSamplerState(0, ShaderScope::PS, point_clamp);

	//depth enabled
	command_list->SetDepthStencilState(depth_stencil_enabled_state);
	command_list->SetRenderTarget(out, render_textures[RenderTargetType::GBuffer_Depth]);
	{
		//grid
		{
			UpdateGlobalBuffer
			(
				out->GetWidth(),
				out->GetHeight(),
				grid->GetComputeWorldMatrix(camera->GetTransform()) * camera_view_proj
			);

			command_list->SetVertexBuffer(grid->GetVertexBuffer());
			command_list->SetIndexBuffer(grid->GetIndexBuffer());
			command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
			command_list->DrawIndexed(grid->GetIndexCount(), 0, 0);
		}

		//line
		{
			const uint vertex_count = static_cast<uint>(depth_enabled_line_vertices.size());
			if (vertex_count)
			{
				if (vertex_count > line_vertex_buffer->GetCount())
				{
					line_vertex_buffer->Clear();
					line_vertex_buffer->Create(depth_enabled_line_vertices, D3D11_USAGE_DYNAMIC);
				}

				auto data = static_cast<VertexColor*>(line_vertex_buffer->Map());
				std::copy(depth_enabled_line_vertices.begin(), depth_enabled_line_vertices.end(), data);
				line_vertex_buffer->Unmap();

				UpdateGlobalBuffer(out->GetWidth(), out->GetHeight(), camera_view_proj);

				command_list->SetVertexBuffer(line_vertex_buffer);
				command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
				command_list->Draw(vertex_count);

				depth_enabled_line_vertices.clear();
			}
		}
	}

	//depth disabled
	command_list->SetDepthStencilState(depth_stencil_disabled_state);
	command_list->SetRenderTarget(out);
	{
		//line
		{
			const uint vertex_count = static_cast<uint>(depth_disabled_line_vertices.size());
			if (vertex_count)
			{
				if (vertex_count > line_vertex_buffer->GetCount())
				{
					line_vertex_buffer->Clear();
					line_vertex_buffer->Create(depth_disabled_line_vertices, D3D11_USAGE_DYNAMIC);
				}

				auto data = static_cast<VertexColor*>(line_vertex_buffer->Map());
				std::copy(depth_disabled_line_vertices.begin(), depth_disabled_line_vertices.end(), data);
				line_vertex_buffer->Unmap();

				UpdateGlobalBuffer(out->GetWidth(), out->GetHeight(), camera_view_proj);

				command_list->SetVertexBuffer(line_vertex_buffer);
				command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
				command_list->Draw(vertex_count);

				depth_disabled_line_vertices.clear();
			}
		}
	}

	command_list->End();
	command_list->Submit();
}

void Renderer::PassDebug(std::shared_ptr<class Texture>& out)
{
	if (debug_buffer_type == Render_Buffer_None) return;

	std::shared_ptr<Texture> texture;
	ShaderType shader_type;

	switch (debug_buffer_type)
	{
	case Render_Buffer_Albedo:
	{
		texture = render_textures[RenderTargetType::GBuffer_Albedo];
		shader_type = ShaderType::PS_TEXTURE;
		break; 
	}
	case Render_Buffer_Normal:
	{
		texture = render_textures[RenderTargetType::GBuffer_Normal];
		shader_type = ShaderType::PS_DEBUG_NORMAL;
		break;
	}
	case Render_Buffer_Material:
		break;
	case Render_Buffer_Diffuse:
		break;
	case Render_Buffer_Specular:
		break;
	case Render_Buffer_Velocity:
		break;
	case Render_Buffer_Depth:
	{
		texture = render_textures[RenderTargetType::GBuffer_Depth];
		shader_type = ShaderType::PS_DEBUG_DEPTH;
		break;
	}
	case Render_Buffer_SSAO:
		break;
	case Render_Buffer_SSR:
		break;
	case Render_Buffer_Bloom:
		break;
	case Render_Buffer_Shadows:
		break;
	default:
		break;
	}

	const auto &vertex_shader = shaders[ShaderType::VS_POST_PROCESS];
	const auto &pixel_shader = shaders[shader_type];
	if (!vertex_shader || !pixel_shader) return;

	command_list->Begin("PassDebug");

	UpdateGlobalBuffer(out->GetWidth(), out->GetHeight(), post_process_view_proj);

	command_list->SetRasterizerState(cull_back_solid);
	command_list->SetBlendState(blend_disabled);
	command_list->SetDepthStencilState(depth_stencil_disabled_state);

	command_list->SetRenderTarget(out);
	command_list->SetViewport(out->GetViewport());
	command_list->SetVertexBuffer(screen_vertex_buffer);
	command_list->SetIndexBuffer(screen_index_buffer);
	command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command_list->SetInputLayout(vertex_shader->GetInputLayout());
	command_list->SetVertexShader(vertex_shader);
	command_list->SetPixelShader(pixel_shader);
	command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
	command_list->SetShaderResource(0, ShaderScope::PS, texture);
	command_list->SetSamplerState(0, ShaderScope::PS, bilinear_clamp);
	command_list->DrawIndexed(screen_index_buffer->GetCount(), screen_index_buffer->GetOffset(), screen_vertex_buffer->GetOffset());
		
	command_list->End();
	command_list->Submit();
}
