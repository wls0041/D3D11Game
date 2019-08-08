#include "Framework.h"
#include "IResource.h"

IResource::IResource(Context * context, const ResourceType & resource_type)
	: context(context)
	, resource_type(resource_type)
	, resource_name(NOT_ASSIGNED_STR)
	, resource_path(NOT_ASSIGNED_STR)
{	 
	resource_id = GUID_Generator::Generate();
}

template<typename T>
constexpr ResourceType IResource::DeduceResourceType() //Ÿ���� ������ ���� �ʾ� h�� �� �� �� ���� cpp�� �ű�
{

	return ResourceType::Unknown;
}

//Explicit template instantiation - ����� ���ø� �ν��Ͻ�(Ư��ȭ)
#define REGISTER_RESOURCE_TYPE(T, enum_type) template <> ResourceType IResource::DeduceResourceType<T>() { return enum_type; }

REGISTER_RESOURCE_TYPE(Texture, ResourceType::Texture)
REGISTER_RESOURCE_TYPE(Model, ResourceType::Model)
REGISTER_RESOURCE_TYPE(Mesh, ResourceType::Mesh)
REGISTER_RESOURCE_TYPE(Material, ResourceType::Material)


