#pragma once
#include "IResource.h"

class Mesh final : public IResource
{
public:
	Mesh(class Context *context);
	~Mesh();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;
private:

};