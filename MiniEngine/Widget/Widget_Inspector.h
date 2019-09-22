#pragma once
#include "IWidget.h"

class Widget_Inspector final : public IWidget
{
public:
	Widget_Inspector(class Context* context);
	~Widget_Inspector() = default;

	void Render() override;

private:
	void ShowTransform(std::shared_ptr<class Transform>& transform) const;
	void ShowScript(std::shared_ptr<class Script>& script);

	void ShowRenderable(std::shared_ptr<class Renderable>& renderable);
	void ShowMaterial(std::shared_ptr<class Material>& material);
	void ShowRigidBody(std::shared_ptr<class RigidBody>& rigid_body);
	void ShowCollider(std::shared_ptr<class Collider>& collider);

private:
	void ShowAddComponentButton();
	void ShowComponentPopup();
	void AddComponentDragDrop();

private:
	std::unique_ptr<ColorPicker> material_color_picker;
};