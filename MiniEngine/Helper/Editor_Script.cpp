#include "stdafx.h"
#include "Editor_Script.h"
#include "ImGui/TextEditor.h"

const std::string Editor_Script::Base_Script_Code =
"class NewScript\n\
{\n\
\tNewScript(Actor @in_actor)\n\
\t{\n\
\t}\n\
\n\
\tvoid Start()\n\
\t{\n\
\t}\n\
\n\
\tvoid Update()\n\
\t{\n\
\t}\n\
}";

namespace Editor_Script_Data
{
	TextEditor editor;
	TextEditor::ErrorMarkers markers;
}

void Editor_Script::Initialize(Context * context)
{
    this->context           = context;
    this->path              = "";
    this->new_name          = "";
    this->is_visible        = false;
    this->is_script_update  = true;

	SetScriptLanguage(ScriptLanguage::AngelScript);
	//SetScript("./ImGui/TextEditor.cpp");
}

void Editor_Script::Render()
{
	if (!is_visible)
		return;

	auto cursor_pos = Editor_Script_Data::editor.GetCursorPosition();

	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Script Editor", &is_visible, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) CreateNewScript();
				if (ImGui::BeginMenu("Open"))
				{
					OpenScript(context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Script));

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save")) SaveToScript();
				if (ImGui::BeginMenu("Save as"))
				{
					if (ImGui::InputText("##Save as", &new_name, ImGuiInputTextFlags_EnterReturnsTrue))
						SaveAsToScript();

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				bool is_read_only = Editor_Script_Data::editor.IsReadOnly();
				if (ImGui::MenuItem("ReadOnly Mode", nullptr, &is_read_only))
					Editor_Script_Data::editor.SetReadOnly(is_read_only);

				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, !is_read_only && Editor_Script_Data::editor.CanUndo()))
					Editor_Script_Data::editor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl + Y", nullptr, !is_read_only && Editor_Script_Data::editor.CanRedo()))
					Editor_Script_Data::editor.Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, Editor_Script_Data::editor.HasSelection()))
					Editor_Script_Data::editor.Copy();
				if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, !is_read_only && Editor_Script_Data::editor.HasSelection()))
					Editor_Script_Data::editor.Cut();
				if (ImGui::MenuItem("Delete", "Del", nullptr, !is_read_only && Editor_Script_Data::editor.HasSelection()))
					Editor_Script_Data::editor.Delete();
				if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, !is_read_only && ImGui::GetClipboardText() != nullptr))
					Editor_Script_Data::editor.Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select All", "Ctrl + A", nullptr))
					Editor_Script_Data::editor.SelectAll();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark"))
					Editor_Script_Data::editor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light"))
					Editor_Script_Data::editor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro Blue"))
					Editor_Script_Data::editor.SetPalette(TextEditor::GetRetroBluePalette());
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Text
		(
			"%6d/%-6d %6d lines | %s | %s | %s | %s",
			cursor_pos.mLine + 1,
			cursor_pos.mColumn + 1,
			Editor_Script_Data::editor.GetTotalLines(),
			Editor_Script_Data::editor.IsOverwrite() ? "Ovr" : "Ins",
			Editor_Script_Data::editor.CanUndo() ? "*" : " ",
			Editor_Script_Data::editor.GetLanguageDefinition().mName.c_str(),
			path.c_str()
		);

		Editor_Script_Data::editor.Render("Script Editor");
	}
	ImGui::End();
}

void Editor_Script::SetScript(const std::string & path)
{
	this->path = path;

	std::ifstream in(path);
	if (in.good())
	{
		std::string str
		(
			(std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>()
		);

		Editor_Script_Data::editor.SetText(str);
	}
}

void Editor_Script::SetScriptLanguage(const ScriptLanguage & language)
{
	switch (language)
	{
	case ScriptLanguage::C:
		Editor_Script_Data::editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
		break;
	case ScriptLanguage::CPlusPlus:
		Editor_Script_Data::editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
		break;
	case ScriptLanguage::AngelScript:
		Editor_Script_Data::editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
		break;
	case ScriptLanguage::HLSL:
		Editor_Script_Data::editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
		break;
	}

	//auto language2 = TextEditor::LanguageDefinition::CPlusPlus();
	//{
	//    static const char* ppnames[] =
	//    {
	//        "NULL",
	//        "PM_REMOVE",
	//        "ZeroMemory",
	//        "DXGI_SWAP_EFFECT_DISCARD",
	//        "D3D_FEATURE_LEVEL",
	//        "D3D_DRIVER_TYPE_HARDWARE",
	//        "WINAPI",
	//        "D3D11_SDK_VERSION",
	//        "assert"
	//    };

	//    static const char* ppvalues[] =
	//    {
	//        "#define NULL ((void*)0)",
	//        "#define PM_REMOVE (0x0001)",
	//        "Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ",
	//        "enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
	//        "enum D3D_FEATURE_LEVEL",
	//        "enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
	//        "#define WINAPI __stdcall",
	//        "#define D3D11_SDK_VERSION (7)",
	//        " #define assert(expression) (void)(                                                  \n"
	//        "    (!!(expression)) ||                                                              \n"
	//        "    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
	//        " )"
	//    };

	//    for (int i = 0; i < ARRAYSIZE(ppnames); i++)
	//    {
	//        TextEditor::Identifier id;
	//        id.mDeclaration = ppvalues[i];
	//        language2.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
	//    }

	//    static const char* identifiers[] =
	//    {
	//        "HWND", "HRESULT", "LPRESULT","D3D11_RENDER_TARGET_VIEW_DESC", "DXGI_SWAP_CHAIN_DESC","MSG","LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//        "ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//        "ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//        "IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "TextEditor"
	//    };

	//    static const char* idecls[] =
	//    {
	//        "typedef HWND_* HWND", "typedef long HRESULT", "typedef long* LPRESULT", "struct D3D11_RENDER_TARGET_VIEW_DESC", "struct DXGI_SWAP_CHAIN_DESC",
	//        "typedef tagMSG MSG\n * Message structure","typedef LONG_PTR LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//        "ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//        "ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//        "IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "class TextEditor"
	//    };

	//    for (int i = 0; i < ARRAYSIZE(identifiers); i++)
	//    {
	//        TextEditor::Identifier id;
	//        id.mDeclaration = std::string(idecls[i]);
	//        language2.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	//    }
	//}
	//Editor_Script_Data::editor.SetLanguageDefinition(language2);

	//Editor_Script_Data::markers.insert(std::make_pair(20, "ERRORRRRRRRRRRRRRRRRRRRRRRRRR!!!"));
	//Editor_Script_Data::markers.insert(std::make_pair(25, "dasfadfadfasdfdddddddddddddd!!!"));
	//Editor_Script_Data::editor.SetErrorMarkers(Editor_Script_Data::markers);
}

void Editor_Script::CreateNewScript()
{
	auto directory = Editor_Helper::Get().resource_manager->GetAssetDirectory(AssetType::Script);

	path = directory + "NewScript.as";

	std::ofstream out;
	out.open(path, std::ios::out);
	if (out.fail())
		return;

	out << Base_Script_Code;
	out.flush();
	out.close();

	SetScript(path);
}

void Editor_Script::OpenScript(const std::string & path)
{
	//fix - 유석
	this->path = path;
	SetScript(path);
}

void Editor_Script::SaveToScript()
{
	//fix - 유석
	std::string temp(Editor_Script_Data::editor.GetText());

	FileSystem::Replace_All(temp, "\r", "");

	std::ofstream out;
	out.open(path, std::ios::out);

	if (out.fail())
		return;

	out << temp;
	out.flush();
	out.close();

	SetScript(path);

	is_script_update = true;
}

void Editor_Script::OpenScript(const std::string & path)
{
	//fix - 유석
	if (is_script_update)
	{
		script_items.clear();
		script_items.shrink_to_fit();

		auto files = FileSystem::GetFilesInDirectory(path);

		for (const auto& file : files)
			script_items.emplace_back(file, *(Icon_Provider::Get().Load(file)));

		is_script_update = false;
	}

	if (ImGui::IsKeyDown(VK_F5))
		is_script_update = true;

	ImGui::BeginChild("##Script_Open", ImVec2(300.0f, 300.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
	{
		int columns = static_cast<int>(ImGui::GetWindowContentRegionWidth() / 100.0f);
		columns = columns < 1 ? 1 : columns;

		ImGui::Columns(columns, nullptr, false);

		for (auto& script_item : script_items)
		{
			//Thumnail
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				if (Icon_Provider::Get().ImageButton(script_item.thumbnail, ImVec2(80.0f, 77.0f)))
				{
					const auto now = std::chrono::high_resolution_clock::now();
					script_item.time_since_last_click = now - script_item.last_click_time;
					script_item.last_click_time = now;

					if (static_cast<float>(script_item.time_since_last_click.count()) < 500.0f)
					{
						this->path = script_item.path;
						SetScript(this->path);
					}
				}

				ImGui::PopStyleColor(2);
			}

			//Label
			{
				auto& g = *GImGui;
				auto& style = ImGui::GetStyle();
				const auto label_height = g.FontSize;

				auto label_size = ImGui::CalcTextSize(script_item.label.c_str(), nullptr, true);
				auto label_pos_min = ImGui::GetCursorScreenPos();
				auto label_pos_max = ImVec2(label_pos_min.x + 90.0f, label_pos_min.y + 15.0f);

				auto label_rect = ImRect
				(
					label_pos_min.x,
					label_pos_max.y - label_height - style.FramePadding.y,
					label_pos_max.x,
					label_pos_max.y
				);

				ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y + 3.0f));

				if (label_size.x <= 63.0f)
				{
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x + 20.0f, label_rect.Min.y));
					ImGui::TextUnformatted(script_item.label.c_str());
				}
				else if (label_size.x >= 64.0f && label_size.x <= 85.0f)
				{
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x + 10.0f, label_rect.Min.y));
					ImGui::TextUnformatted(script_item.label.c_str());
				}
				else
				{
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x - 20.0f, label_rect.Min.y + 30.0f));
					ImGui::RenderTextClipped
					(
						label_rect.Min,
						label_rect.Max,
						script_item.label.c_str(),
						nullptr,
						&label_size,
						ImVec2(0, 0),
						&label_rect
					);
				}
			}
			ImGui::NextColumn();
		}
	}
	ImGui::EndChild();
}

void Editor_Script::SaveAsToScript()
{
	//fix - 유석
	if (new_name.size() < 1 || this->path.size() < 1)
	{
		new_name = "";
		return;
	}

	std::string old_name = path;
	path = "../../_Assets/Script/";

	new_name = path + new_name + ".as";

	std::string temp(Editor_Script_Data::editor.GetText());

	FileSystem::Replace_All(temp, FileSystem::GetIntactFileNameFromPath(old_name), FileSystem::GetIntactFileNameFromPath(new_name));
	FileSystem::Replace_All(temp, "\r", "");

	std::ofstream out;
	out.open(new_name, std::ios::out);

	if (out.fail())
		return;

	out << temp;
	out.flush();
	out.close();

	path = old_name;
	new_name = "";

	is_script_update = true;
}
