#include "stdafx.h"
#include "ColorPicker.h"

namespace _ColorPicker
{
    static std::string button_label;
    static std::string color_picker_label;
    static bool show_wheel = false;
    static bool show_preview = true;
    static bool hdr = false;
    static bool alpha_preview = true;
    static bool alpha_half_preview = false;
    static bool option_menu = true;
    static bool show_RGB = true;
    static bool show_HSV = false; // Hue - 색조, Saturation - 채도, Value - 명도
    static bool show_HEX = true;
}

ColorPicker::ColorPicker(const std::string & title)
{
    this->title = title;
    this->is_visible = false;
    this->color = Color4::Black;
    _ColorPicker::button_label = "##" + title;
    _ColorPicker::color_picker_label = "##" + title;
}

void ColorPicker::Update()
{
    if (ImGui::ColorButton(_ColorPicker::button_label.c_str(), ImVec4(color.r, color.g, color.b, color.a)))
        is_visible = true;

    if (is_visible)
        ShowColorPicker();
}

void ColorPicker::ShowColorPicker()
{
    ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title.c_str(), &is_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFocus();

    int misc_flags =
        (_ColorPicker::hdr ? ImGuiColorEditFlags_HDR : 0) |
        (_ColorPicker::alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (_ColorPicker::alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
        (_ColorPicker::option_menu ? 0 : ImGuiColorEditFlags_NoOptions);

    ImGuiColorEditFlags                 flags = misc_flags;
    flags |= ImGuiColorEditFlags_AlphaBar;
    if (!_ColorPicker::show_preview)    flags |= ImGuiColorEditFlags_NoSidePreview;
    if (_ColorPicker::show_wheel)       flags |= ImGuiColorEditFlags_PickerHueWheel;
    if (_ColorPicker::show_RGB)         flags |= ImGuiColorEditFlags_DisplayRGB;
    if (_ColorPicker::show_HSV)         flags |= ImGuiColorEditFlags_DisplayHSV;
    if (_ColorPicker::show_HEX)         flags |= ImGuiColorEditFlags_DisplayHex;

    ImGui::ColorPicker4(_ColorPicker::color_picker_label.c_str(), color, flags);
    ImGui::Separator();

    //Wheel
    ImGui::TextUnformatted("Wheel");
    ImGui::SameLine();
    ImGui::Checkbox("##ColorPicker_Wheel", &_ColorPicker::show_wheel);

    //RGB
    ImGui::TextUnformatted("RGB");
    ImGui::SameLine();
    ImGui::Checkbox("##ColorPicker_RGB", &_ColorPicker::show_RGB);

    //HSV
    ImGui::TextUnformatted("HSV");
    ImGui::SameLine();
    ImGui::Checkbox("##ColorPicker_HSV", &_ColorPicker::show_HSV);

    //HEX
    ImGui::TextUnformatted("HEX");
    ImGui::SameLine();
    ImGui::Checkbox("##ColorPicker_HEX", &_ColorPicker::show_HEX);

    ImGui::End();
}
