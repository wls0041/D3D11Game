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

	ShowTop(is_visible); //번갈아 보여주기 위해 나눠놓음
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

void FileDialog::SetCallBackOnItemClick(const std::function<void(const std::string&)>& callback) //function. 함수 포인터. <타입(매개변수)>
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
		//접기x, 스크롤바
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
	const auto window = ImGui::GetCurrentWindowRead(); //윈도우 받아옴
	const auto content_width = ImGui::GetContentRegionAvail().x; 
	const auto content_height = ImGui::GetContentRegionAvail().y - (type == FileDialogType::Browser ? 30.0f : 0.0f);

	auto& g = *GImGui; //imgui contex받아옴
	auto& style = ImGui::GetStyle();
	const float font_scale = item_size.x / 100.0f; //font 스케일 설정
	const float label_height = g.FontSize * font_scale; 
	const float text_offset = 3.0f;
	auto check = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f); //외각선 두께 설정
	if (check = ImGui::BeginChild("##ContentRegion", ImVec2(content_width, content_height), true))
	{
		//dialog의 hovered판단
		Dialog::is_hovered_window = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		auto pen_x = 0.0f;
		auto new_line = true;

		for (int i = 0; i < static_cast<int>(items.size()); i++)
		{
			if (new_line)//new_line이 존재하면
			{
				ImGui::BeginGroup(); //그룹화
				new_line = false; //new_line초기화
			}

			ImGui::BeginGroup();
			{
				auto& item = items[i];
				auto button_pos_min = ImGui::GetCursorScreenPos(); //커서위치부터 버튼 만듦 
				auto button_pos_max = ImVec2(button_pos_min.x + item_size.x, button_pos_min.y + item_size.y); //버튼 범위 설정

				//Tumbnail
				{
					ImGui::PushID(i); //id설정
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));//외곽선 색 설정
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));//버튼 색 설정
					{
						if (ImGui::Button("##dummy", item_size))
						{
							item.Clicked(); //아이템 클릭 가부 체크
							const auto is_single_click = item.GetTimeSinceLastClickedMs() > 500; //시간 체크로 싱글클릭인지 확인

							if (is_single_click)//맞다면
							{
								input_box = item.GetLabel(); //아이템라벨 가져오기
								if (on_item_clicked) //아이템 클릭되면
									on_item_clicked(item.GetPath()); //경로 저장
							}
							else
							{
								is_update = SetDialogCurrentPath(item.GetPath()); //반환되는 bool값을 통해 update되었는지 확인
								is_selection = !item.IsDirectory(); //item directory인지 체크

								if (on_item_double_clicked) //더블클릭
									on_item_double_clicked(current_directory); //경로저장
							}
						}

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) //hovered 체크
						{
							Dialog::is_hovered_item = true;
							Dialog::hovered_item_path = item.GetPath(); //경로 저장
						}

						ItemClick(&item);
						ItemMenu(&item);
						ItemDrag(&item);

						ImGui::SetCursorScreenPos(button_pos_min + style.FramePadding); //커서위치 조정해서 넣음
						ImGui::Image //설정된 대로 출력
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
					auto label_text = item.GetLabel().c_str(); //text설정
					auto label_size = ImGui::CalcTextSize(label_text, nullptr, true); //사이즈 설정
					auto label_rect = ImRect //렉트 크기 설정
					(
						button_pos_min.x,
						button_pos_max.y - label_height - style.FramePadding.y,
						button_pos_max.x,
						button_pos_max.y
					);

					ImGui::GetWindowDrawList()->AddRectFilled //색 채우기
					(
						label_rect.Min,
						label_rect.Max,
						IM_COL32(255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.75f)
					);

					ImGui::SetWindowFontScale(font_scale);
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y + text_offset));
					if (label_size.x <= item_size.x) //아이템 사이즈가 label사이즈보다 크면
						ImGui::TextUnformatted(label_text); //label텍스트 씀
					else
						ImGui::RenderTextClipped //label이 더 크면 label재설정
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

			pen_x += item_size.x + style.ItemSpacing.x; //펜 가로위치 확인
			if (pen_x >= content_width - item_size.x) //공간이 끝나면
			{
				ImGui::EndGroup(); //그룹 끝내고 
				pen_x = 0; //위치 초기화
				new_line = true; //새 줄 시작
			}
			else //아니면 Line유지
				ImGui::SameLine();
		}

		if (!new_line) //line 새로 만든건지 확인
			ImGui::EndGroup();

		ImGui::EndChild();
	}

	if (!check) //위 if문의 처음에서 잘 만들어졌는지 확인
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

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) //id 없어도 dragdrop가능하게 함
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
	if (!FileSystem::IsDirectory(path)) //경로가 필요한게 아니라 경로 들어오면 내보냄
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
