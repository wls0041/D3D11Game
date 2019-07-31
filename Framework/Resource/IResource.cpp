#include "Framework.h"
#include "IResource.h"

IResource::IResource(Context * context, const ResourceType & resource_type)
	: context(context)
	, resource_type(resource_type)
	, resource_name(NOT_ASSIGNED_STR)
	, resource_path(NOT_ASSIGNED_STR)
{	 
	id = GUID_Generator::Generate();
}

template<typename T>
constexpr ResourceType IResource::DeduceResourceType() //타입이 정해져 있지 않아 h에 쓸 수 가 없어 cpp로 옮김
{

	return ResourceType::Unknown;
}

//Explicit template instantiation - 명시적 템플릿 인스턴스(특수화)
#define REGISTER_RESOURCE_TYPE(T, enum_type) template <> ResourceType IResource::DeduceResourceType<T>() { return enum_type; }

REGISTER_RESOURCE_TYPE(Texture, ResourceType::Texture)
