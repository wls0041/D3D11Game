#include "Framework.h"
#include "CommandList.h"

Command::Command()
{
	constant_buffers.reserve(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT); //constantbuffer 한계 개수 14개
	shader_resources.reserve(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT); //128개
	samplers.reserve(D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT); //16개
	render_targets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT); //8개

	Clear();
}

void Command::Clear()
{    command_type                    = CommandType::Unknown;
    pass_name                       = NOT_ASSIGNED_STR;

    vertex_buffer                   = nullptr;
    vertex_count                    = 0;
    vertex_offset                   = 0;
    vertex_stride                   = 0;

    index_buffer                    = nullptr;
    index_count                     = 0;
    index_offset                    = 0;

    input_layout                    = nullptr;
    primitive_topology              = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    vertex_shader                   = nullptr;
    pixel_shader                    = nullptr;
    hull_shader                     = nullptr;
    domain_shader					= nullptr;
    geometry_shader                 = nullptr;
    compute_shader                  = nullptr;

    constant_buffers.clear();
    constant_buffer_shader_scope    = ShaderScope::Unknown;
    constant_buffer_count           = 0;
    constant_buffer_slot            = 0;

    shader_resources.clear();
    shader_resource_shader_scope    = ShaderScope::Unknown;
    shader_resource_count           = 0;
    shader_resource_slot            = 0;

    samplers.clear();
    sampler_shader_scope            = ShaderScope::Unknown;
    sampler_count                   = 0;
    sampler_slot                    = 0;

    rasterizer_state                = nullptr;
    viewport                        = { 0 };

    blend_state                     = nullptr;
    depth_stencil_state             = nullptr;

    render_targets.clear();
    render_target_count             = 0;

    depth_stencil_target            = nullptr;

    clear_render_target             = nullptr;
    clear_color                     = Color4::Zero;

    clear_depth_stencil_target      = nullptr;
    clear_flags                     = 0;
    clear_stencil                   = 0;
    clear_depth                     = 0.0f;
}

CommandList::CommandList(Context * context)
	: command_capacity(3000)
	, command_count(0)
{
	commands.reserve(command_capacity); //재할당 필요없어 속도가 빨라짐
	commands.resize(command_capacity);

	graphics = context->GetSubsystem<Graphics>();
}

CommandList::~CommandList()
{
	Clear();
}

auto CommandList::GetCommand() -> Command &
{
	if (command_count >= static_cast<uint>(commands.size())) {
		uint new_size = command_count + 100;
		commands.reserve(new_size);
		commands.resize(new_size);

		LOG_WARNING_F("Command List has been fulled %d command. Consider to make the larger capacity to avoid re-allocation", command_count);
	}
	return commands[command_count++];
}

void CommandList::Clear()
{
}
