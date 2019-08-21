#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Actor.h"
#include "../../Scene/Component/Camera.h"
#include "../../Scene/Component/Transform.h"
#include "../../Scene/Component/Renderable.h"

void Renderer::PassMain()
{
	command_list->Begin("PassMain");
	{
		PassGBuffer();
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
			command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH, 1.0f); //depth는 렌더타겟이 없음. 1.0 - 추후 값 수정할 것임. z축 바뀔 때 뒤집힘
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
			
			//TODO : Rasterizer State

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
				command_list->SetShaderResources(0, ShaderScope::PS, material->GetTextureShaderResource());
				
				material->UpdateConstantBuffer(); //Material에 만들어 놓은 roughtness, metallic 등의 요소 업데이트
				command_list->SetConstantBuffer(1, ShaderScope::PS, material->GetConstantBuffer());
				current_material_id = material->GetResourceID();
			}

			const auto &transform = actor->GetTransform();
			transform->UpdateConstantBuffer(camera_view_proj); //Gbuffer.hlsl의 material_buffer, model_buffer 업데이트
			command_list->SetConstantBuffer(2, ShaderScope::VS, transform->GetConstantBuffer());

			command_list->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), mesh->GetIndexBuffer()->GetOffset(), mesh->GetVertexBuffer()->GetCount());
		};

		command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command_list->SetRenderTargets(render_targets, depth_texture->GetDepthStencilView());
		command_list->SetViewport(albedo_texture->GetViewport());
		command_list->ClearRenderTargets(render_targets);
		command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH, 1.0f);
		command_list->SetVertexShader(vertex_shader);
		command_list->SetInputLayout(vertex_shader->GetInputLayout());
		command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
	
		for (const auto &actor : renderables[RenderableType::Opaque])
			draw_actor(actor);

		//TODO : blend state 만들고 처리

		for (const auto &actor : renderables[RenderableType::Transparent])
			draw_actor(actor);
	}
	command_list->End();
	command_list->Submit();
}
