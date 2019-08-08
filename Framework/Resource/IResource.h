#pragma once

enum class ResourceType : uint {
	Unknown, 
	Texture,
	Model,
	Material,
	Mesh,
};

class IResource
{
public:
	template <typename T>
	static constexpr ResourceType DeduceResourceType(); //�ζ��� �Լ��� ����� �����Ƿ� ���������� ���� ��� �����Ϸ��� ������

public:
	IResource(class Context *context, const ResourceType &resource_type = ResourceType::Unknown);
	virtual ~IResource() = default;

	virtual const bool SaveToFile(const std::string &path) = 0;
	virtual const bool LoadFromFile(const std::string &path) = 0;

	auto GetResourceID() const -> const uint& { return resource_id; }

	auto GetResourceType() const -> const ResourceType& { return resource_type; }
	void SetResourceType(const ResourceType &type) { this->resource_type = type; }

	auto GetResourceName() const -> const std::string& { return resource_name; }
	void SetResourceName(const std::string &name) { this->resource_name = name; }

	auto GetResourcePath() const -> const std::string& { return resource_path; }
	void SetResourcePath(const std::string &path) { this->resource_path = path; }

protected:
	class Context* context;

	uint resource_id;
	ResourceType resource_type;
	std::string resource_name;
	std::string resource_path;
	
};
