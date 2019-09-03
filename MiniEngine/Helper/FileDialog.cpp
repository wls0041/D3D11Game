#include "stdafx.h"
#include "FileDialog.h"

namespace Dialog
{
	static float item_size_min = 50.0f;
	static float item_size_max = 200.0f;
	static bool is_hovered_item;
	static bool is_hovered_window;
	static std::string hovered_item_path;
	static uint popup_id;
	static DragDropPayload payload;
}

#define OPERATION_NAME  (operation == FileDialogOperation::Open) ? "Open" : (operation == FileDialogOperation::Load) ? "Load" : (operation == FileDialogOperation::Save) ? "Save" : "View"
#define FILTER_NAME     (filter == FileDialogFilter::All) ? "All(*.*)" : (filter == FileDialogFilter::Model) ? "Model(*.model)" : (filter == FileDialogFilter::Texture) ? "Texture(*.texture)" : "Scene(*.scene)"

FileDialog::FileDialog(Context * context, const FileDialogType & type, const FileDialogOperation & operation, const FileDialogFilter & filter, const bool & is_windowed)
	: context(context)
	, type(type)
	, operation(operation)
	, filter(filter)
	, is_window(is_windowed)
	, item_size(100.0f, 100.0f)
	, is_update(true)
	, is_selection(false)
	, on_item_clicked(nullptr)
	, on_item_double_clicked(nullptr) 
{
    title               = OPERATION_NAME;
    current_directory   = Editor_Helper::Get().resource_manager->GetAssetDirectory();
}

void FileDialog::SetOperation(const FileDialogOperation & operation)
{
    if (this->operation == operation) return;

    this->operation = operation;
    this->title     = OPERATION_NAME;
}

auto FileDialog::Show(bool * is_visible, std::string * directory, std::string * path) -> const bool
{
	if (!(*is_visible))
	{
		is_update = true;
		return false;
	}

	is_selection = false;
	Dialog::is_hovered_item = false;
	Dialog::is_hovered_window = false;

	ShowTop(is_visible); //������ �����ֱ� ���� ��������
	ShowMiddle();
	ShowBottom(is_visible);

	if (is_window) ImGui::End();

	if (is_update)
	{
		UpdateDialogFromDirectory(current_directory);
		is_update = false;
	}

	if (is_selection)
	{
		if (directory) (*directory) = current_directory; 
		if (path)	   (*path) = current_directory + "/" + std::string(input_box);
	}

	MenuPopup();

	return is_selection;
}

void FileDialog::SetCallBackOnItemClick(const std::function<void(const std::string&)>& callback) //function. �Լ� ������. <Ÿ��(�Ű�����)>
{
	on_item_clicked = callback;
}

void FileDialog::SetCallBackOnItemDoubleClick(const std::function<void(const std::string&)>& callback)
{
	on_item_double_clicked = callback;
}

void FileDialog::ShowTop(bool * is_visible)
{
	if (is_window)
	{
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints
		(
			ImVec2(350.0f, 250.0f),
			ImVec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
		);
		//����x, ��ũ�ѹ�
		ImGui::Begin(title.c_str(), is_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDocking);
		ImGui::SetWindowFocus();
	}

	if (ImGui::ArrowButton("left_button", ImGuiDir_Left))
	{
		SetDialogCurrentPath(FileSystem::GetParentDirectory(current_directory));
		is_update = true;
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(current_directory.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.8f);
	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.2f);
	{
		float privious_width = item_size.x;
		ImGui::SliderFloat("##FileDialogSlider", &item_size.x, Dialog::item_size_min, Dialog::item_size_max);
		item_size.y += item_size.x - privious_width;
	}
	ImGui::PopItemWidth();
	ImGui::Separator();
}

void FileDialog::ShowMiddle()
{
	const auto window = ImGui::GetCurrentWindowRead(); //������ �޾ƿ�
	const auto content_width = ImGui::GetContentRegionAvail().x; 
	const auto content_height = ImGui::GetContentRegionAvail().y - (type == FileDialogType::Browser ? 30.0f : 0.0f);

	auto& g = *GImGui; //imgui contex�޾ƿ�
	auto& style = ImGui::GetStyle();
	const float font_scale = item_size.x / 100.0f; //font ������ ����
	const float label_height = g.FontSize * font_scale; 
	const float text_offset = 3.0f;
	auto check = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f); //�ܰ��� �β� ����
	if (check = ImGui::BeginChild("##ContentRegion", ImVec2(content_width, content_height), true))
	{
		//dialog�� hovered�Ǵ�
		Dialog::is_hovered_window = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		auto pen_x = 0.0f;
		auto new_line = true;

		for (int i = 0; i < static_cast<int>(items.size()); i++)
		{
			if (new_line)//new_line�� �����ϸ�
			{
				ImGui::BeginGroup(); //�׷�ȭ
				new_line = false; //new_line�ʱ�ȭ
			}

			ImGui::BeginGroup();
			{
				auto& item = items[i];
				auto button_pos_min = ImGui::GetCursorScreenPos(); //Ŀ����ġ���� ��ư ���� 
				auto button_pos_max = ImVec2(button_pos_min.x + item_size.x, button_pos_min.y + item_size.y); //��ư ���� ����

				//Tumbnail
				{
					ImGui::PushID(i); //id����
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));//�ܰ��� �� ����
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));//��ư �� ����
					{
						if (ImGui::Button("##dummy", item_size))
						{
							item.Clicked(); //������ Ŭ�� ���� üũ
							const auto is_single_click = item.GetTimeSinceLastClickedMs() > 500; //�ð� üũ�� �̱�Ŭ������ Ȯ��

							if (is_single_click)//�´ٸ�
							{
								input_box = item.GetLabel(); //�����۶� ��������
								if (on_item_clicked) //������ Ŭ���Ǹ�
									on_item_clicked(item.GetPath()); //��� ����
							}
							else
							{
								is_update = SetDialogCurrentPath(item.GetPath()); //��ȯ�Ǵ� bool���� ���� update�Ǿ����� Ȯ��
								is_selection = !item.IsDirectory(); //item directory���� üũ

								if (on_item_double_clicked) //����Ŭ��
									on_item_double_clicked(current_directory); //�������
							}
						}

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) //hovered üũ
						{
							Dialog::is_hovered_item = true;
							Dialog::hovered_item_path = item.GetPath(); //��� ����
						}

						ItemClick(&item);
						ItemMenu(&item);
						ItemDrag(&item);

						ImGui::SetCursorScreenPos(button_pos_min + style.FramePadding); //Ŀ����ġ �����ؼ� ����
						ImGui::Image //������ ��� ���
						(
							item.GetTexture()->GetShaderResourceView(),
							ImVec2
							(
								button_pos_max.x - button_pos_min.x - style.FramePadding.x * 2.0f,
								button_pos_max.y - button_pos_min.y - style.FramePadding.y - label_height - 5.0f
							)
						);
					}
					ImGui::PopStyleColor(2);
					ImGui::PopID();
				}

				//Label
				{
					auto label_text = item.GetLabel().c_str(); //text����
					auto label_size = ImGui::CalcTextSize(label_text, nullptr, true); //������ ����
					auto label_rect = ImRect //��Ʈ ũ�� ����
					(
						button_pos_min.x,
						button_pos_max.y - label_height - style.FramePadding.y,
						button_pos_max.x,
						button_pos_max.y
					);

					ImGui::GetWindowDrawList()->AddRectFilled //�� ä���
					(
						label_rect.Min,
						label_rect.Max,
						IM_COL32(255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.75f)
					);

					ImGui::SetWindowFontScale(font_scale);
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y + text_offset));
					if (label_size.x <= item_size.x) //������ ����� label������� ũ��
						ImGui::TextUnformatted(label_text); //label�ؽ�Ʈ ��
					else
						ImGui::RenderTextClipped //label�� �� ũ�� label�缳��
						(
							label_rect.Min,
							label_rect.Max,
							label_text,
							nullptr,
							&label_size,
							ImVec2(0, 0),
							&label_rect
						);
				}
			}
			ImGui::EndGroup();

			pen_x += item_size.x + style.ItemSpacing.x; //�� ������ġ Ȯ��
			if (pen_x >= content_width - item_size.x) //������ ������
			{
				ImGui::EndGroup(); //�׷� ������ 
				pen_x = 0; //��ġ �ʱ�ȭ
				new_line = true; //�� �� ����
			}
			else //�ƴϸ� Line����
				ImGui::SameLine();
		}

		if (!new_line) //line ���� ������� Ȯ��
			ImGui::EndGroup();

		ImGui::EndChild();
	}

	if (!check) //�� if���� ó������ �� ����������� Ȯ��
		ImGui::EndChild();

	ImGui::PopStyleVar();
}

void FileDialog::ShowBottom(bool * is_visible)
{
	if (type == FileDialogType::Browser)
		return;

	ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 35.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);

	ImGui::PushItemWidth(ImGui::GetWindowSize().x - 235.0f);
	{
		ImGui::InputText("##input_box", &input_box);
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::TextUnformatted(FILTER_NAME);

	ImGui::SameLine();
	if (ImGui::Button(OPERATION_NAME))
		is_selection = true;

	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		is_selection = false;
		(*is_visible) = false;
	}
}

void FileDialog::ItemDrag(FileDialogItem * item) const
{
	if (!item || type != FileDialogType::Browser) return;

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) //id ��� dragdrop�����ϰ� ��
	{
		const auto set_payload = [](const PayloadType& type, const std::string& path)
		{
			Dialog::payload.type = type;
			Dialog::payload.data = path.c_str();
			DragDropEvent::CreateDragDropPayload(Dialog::payload);
		};

		if (FileSystem::IsSupportModelFile(item->GetPath()))	set_payload(PayloadType::Model, item->GetPath());
		if (FileSystem::IsSupportTextureFile(item->GetPath()))  set_payload(PayloadType::Texture, item->GetPath());
		if (FileSystem::IsSupportScriptFile(item->GetPath()))	set_payload(PayloadType::Script, item->GetPath());

		ImGui::Image(item->GetTexture()->GetShaderResourceView(), ImVec2(50.0f, 50.0f));

		ImGui::EndDragDropSource();
	}
}

void FileDialog::ItemClick(FileDialogItem * item) const
{
	if (!item || !Dialog::is_hovered_window) return; 
	if (ImGui::IsItemClicked(1))
	{
		Dialog::popup_id = item->GetID();
		ImGui::OpenPopup("##FileDialogPopup");
	}
}

void FileDialog::ItemMenu(FileDialogItem * item)
{
	if (Dialog::popup_id != item->GetID()) return; 
	if (!ImGui::BeginPopup("##FileDialogPopup")) return;

	if (ImGui::MenuItem("Delete"))
	{
		if (item->IsDirectory())
		{
			FileSystem::Delete_Directory(item->GetPath());
			is_update = true;
		}
		else
		{
			FileSystem::Delete_File(item->GetPath());
			is_update = true;
		}
	}

	ImGui::Separator();

	if (ImGui::MenuItem("File Explorer")) FileSystem::OpenDirectoryWindow(item->GetPath());

	ImGui::EndPopup();
}

auto FileDialog::SetDialogCurrentPath(const std::string & path) -> const bool
{
	if (!FileSystem::IsDirectory(path)) return false;

	current_directory = path;
	return true;
}

auto FileDialog::UpdateDialogFromDirectory(const std::string & path) -> const bool
{
	if (!FileSystem::IsDirectory(path)) //��ΰ� �ʿ��Ѱ� �ƴ϶� ��� ������ ������
	{
		LOG_ERROR("Invalid parameter");
		return false;
	}

	items.clear();
	items.shrink_to_fit();

	auto directories = FileSystem::GetDirectoriesInDirectory(path);
	for (const auto& directory : directories)
		items.emplace_back(directory, *Icon_Provider::Get().Load(directory, IconType::Thumbnail_Folder));

	//Get files
	std::vector<std::string> files;
	switch (filter)
	{
	case FileDialogFilter::All:
	{
		files = FileSystem::GetFilesInDirectory(path);
		for (const auto& file : files)
			items.emplace_back(file, *Icon_Provider::Get().Load(file, IconType::Thumbnail_Custom));
		break;
	}
	case FileDialogFilter::Scene:
		break;
	case FileDialogFilter::Model:
	{
		files = FileSystem::GetSupportModelFilesInDirectory(path);
		for (const auto& file : files)
			items.emplace_back(file, *Icon_Provider::Get().Load(file, IconType::Thumbnail_Custom));
		break;
	}
	case FileDialogFilter::Texture:
		files = FileSystem::GetSupportTextureFilesInDirectory(path);
		for (const auto& file : files)
			items.emplace_back(file, *Icon_Provider::Get().Load(file, IconType::Thumbnail_Custom));
		break;
	}

	return true;
}

void FileDialog::MenuPopup()
{
	if (ImGui::IsMouseClicked(1) && Dialog::is_hovered_window && !Dialog::is_hovered_item) ImGui::OpenPopup("##MenuPopup");
	if (!ImGui::BeginPopup("##MenuPopup")) return;
	if (ImGui::MenuItem("Create Folder"))
	{
		FileSystem::Create_Directory(current_directory + "New_Folder");
		is_update = true;
	}
	if (ImGui::MenuItem("File Explorer")) FileSystem::OpenDirectoryWindow(current_directory);

	ImGui::EndPopup();
}
