#pragma once
#include "stdafx.h"

enum class PayloadType : uint
{
	Unknown,
	Texture,
	Model,
	Actor,
	Audio,
	Script,
};

struct DragDropPayload
{
	typedef std::variant<const char*, uint> data_variant;

	DragDropPayload(const PayloadType& type = PayloadType::Unknown, const data_variant& data = nullptr)
		: type(type)
		, data(data)
	{}

	PayloadType type;
	data_variant data;
};

class DragDropEvent final
{
public:
	static void CreateDragDropPayload(const DragDropPayload& payload)
	{
		ImGui::SetDragDropPayload
		(
			reinterpret_cast<const char*>(&payload.type),
			reinterpret_cast<const void*>(&payload),
			sizeof(payload),
			ImGuiCond_Once
		);
	}

	static auto ReceiveDragDropPayload(const PayloadType& type) -> DragDropPayload*
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
				return static_cast<DragDropPayload*>(payload->Data);

			ImGui::EndDragDropTarget();
		}
		return nullptr;
	}
};