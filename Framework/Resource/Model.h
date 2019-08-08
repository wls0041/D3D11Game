#pragma once
#include "IResource.h"

class Model final : public IResource
{
public:
	Model(class Context *context);
	~Model();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;

private:

};