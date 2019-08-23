#pragma once
#include "IWidget.h"

class Widget_Hierarchy final : public IWidget
{
public:
    Widget_Hierarchy(class Context* context);
    ~Widget_Hierarchy() = default;

    void Render() override;

private:
	void ShowHierarchy();
	void ShowPopup();

private:
	void AddActor(const  std::shared_ptr<class Actor> &actor);

private:
	auto CreateEmpty() -> std::shared_ptr<class Actor>;
	void CreateStandardMesh(const MeshType &type);
};