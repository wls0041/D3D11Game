#pragma once

enum class ResourceType : uint {
	Unknown, 
	Texture,

};

class IResource
{
public:
	template <typename T>
	static constexpr ResourceType DeduceResourceType(); //인라인 함수가 헤더에 없으므로 문법적으로 오류 없어도 컴파일러가 에러냄

public:
	IResource(class Context *context, const ResourceType &resource_type = ResourceType::Unknown);
	virtual ~IResource() = default;

	auto GetResourceID() const -> const uint& { return resource_id; }

	auto GetResourceType() const -> const ResourceType& { return resource_type; }
	void SetResourceType(const ResourceType &type) { this->resource_type = type; }

	auto GetResourceName() const -> const std::string& { return resource_name; }
	void SetResourceName(const std::string &name) { this->resource_name = name; }

	auto GetResourcePath() const -> const std::string& { return resource_path; }
	void SetResourcePath(const std::string &path) { this->resource_path = path; }

private:
	class Context *context;
	ResourceType resource_type;

	uint resource_id;

	std::string resource_name;
	std::string resource_path;
	
};
