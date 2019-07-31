#include "Framework.h"
#include "Texture.h"
#include "./Importer/TextureImporter.h"

Texture::Texture(Context * context)
    : IResource(context, ResourceType::Texture)
    , bpp(0)
    , bpc(8)
    , width(0)
    , height(0)
    , channels(0)
    , array_size(0)
    , is_gray_scale(false)
    , is_transparent(false)
    , is_generate_mip_chain(true)
    , format(DXGI_FORMAT_R8G8B8A8_UNORM)
	, bind_flags(TEXTURE_BIND_FLAGS::SRV)
    , shader_resource_view(nullptr)
    , unordered_access_view(nullptr)
    , render_target_view(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
}

Texture::~Texture()
{
    SAFE_RELEASE(shader_resource_view);
    SAFE_RELEASE(unordered_access_view);
    SAFE_RELEASE(render_target_view);

    for (auto depth_stencil_view : depth_stencil_views)
        SAFE_RELEASE(depth_stencil_view);
}

const bool Texture::SaveToFile(const std::string & path)
{
	uint byte_count = 0;

	if (FileSystem::IsExistFile(path)) {
		auto stream = std::make_unique <FileStream>(path, Stream_Read);
		if (stream->IsOpen()) stream->Read(byte_count);
	}

	auto append = true;
	auto stream = std::make_unique <FileStream>(path, Stream_Write | Stream_Append);
	if (!stream->IsOpen()) return false;

	//binary data에 저장되어 있는 순서 = byte_count(개수), mip_count, mipData, bpp, bpc, ...
	if (byte_count != 0 && mip_chain.empty()) 
		stream->Skip(sizeof(uint) + sizeof(uint) + byte_count); //byte_count 건너뛰고 밉채인 사이즈 건너뛰고 저장된 내용
	else {
		byte_count = GetByteCount();
		stream->Write(byte_count);
		stream->Write(static_cast<uint>(mip_chain.size()));

		for (auto &mip : mip_chain) stream->Write(mip);

		mip_chain.clear(); //이미 srv에 찍고 바이너리 파일로 빼놨기 때문에 굳이 들고있을 필요 없이 지워버리고 필요할 때 불러서 씀
		mip_chain.shrink_to_fit();
	}

	stream->Write(bpp);
	stream->Write(bpc);
	stream->Write(width);
	stream->Write(height);
	stream->Write(channels);
	stream->Write(is_gray_scale);
	stream->Write(is_transparent);
	stream->Write(resource_name);
	stream->Write(resource_path);

	return true;
}

const bool Texture::LoadFromFile(const std::string & path)
{
	return false;
}

void Texture::SetViewport(const float & x, const float & y, const float & width, const float & height)
{
    viewport.TopLeftX   = x;
    viewport.TopLeftY   = y;
    viewport.Width      = width;
    viewport.Height     = height;
    viewport.MinDepth   = 0.0f;
    viewport.MaxDepth   = 1.0f;
}

void Texture::GetMipChain(std::vector<mip_level>* mip_chain)
{
	if (!this->mip_chain.empty()) {
		mip_chain = &this->mip_chain;
		return;
	}

	auto stream = std::make_unique<FileStream>(resource_path, Stream_Read);
	if (!stream->IsOpen()) return;

	stream->Skip(sizeof(uint));

	uint mip_count= stream->Read<uint>();
	this->mip_chain.reserve(mip_count);
	this->mip_chain.resize(mip_count);

	for (auto &mip : this->mip_chain)  stream->Read(mip);
}

void Texture::ClearMipChain()
{
	for (auto& mip : mip_chain)
	{
		mip.clear();
		mip.shrink_to_fit();
	}

	mip_chain.clear();
	mip_chain.shrink_to_fit();
}

auto Texture::GetMipLevel(const uint & level) -> mip_level *
{
	if (level >= mip_chain.size())
	{
		LOG_ERROR("Mip Level out of range");
		return nullptr;
	}

	return &mip_chain[level];
}

auto Texture::Load_Foreign(const std::string & path, const bool & is_generate_mip_chain) -> const bool
{
	return false;
}

auto Texture::Load_Native(const std::string & path) -> const bool
{
	auto stream = std::make_unique<FileStream>(path, Stream_Read);
	if (!stream->IsOpen()) return false;

	mip_chain.clear();
	mip_chain.shrink_to_fit();

	auto byte_count = stream->Read<uint>();
	auto mip_count = stream->Read<uint>();

	for (auto &mip : mip_chain) stream->Read(mip);
	
	stream->Write(bpp);
	stream->Write(bpc);
	stream->Write(width);
	stream->Write(height);
	stream->Write(channels);
	stream->Write(is_gray_scale);
	stream->Write(is_transparent);
	stream->Write(resource_name);
	stream->Write(resource_path);

	return true;
}

auto Texture::GetByteCount() -> const uint
{
	uint byte_count = 0;

	for (auto &mip : mip_chain) byte_count += static_cast<uint>(mip.size());
	
	return byte_count;
}

auto Texture::GetChannelsFromFormat(const DXGI_FORMAT & format) -> const uint
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return 4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:    return 4;
	case DXGI_FORMAT_R32G32B32A32_UINT:     return 4;
	case DXGI_FORMAT_R32G32B32A32_SINT:     return 4;
	case DXGI_FORMAT_R32G32B32_TYPELESS:    return 3;
	case DXGI_FORMAT_R32G32B32_FLOAT:       return 3;
	case DXGI_FORMAT_R32G32B32_UINT:        return 3;
	case DXGI_FORMAT_R32G32B32_SINT:        return 3;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return 4;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:    return 4;
	case DXGI_FORMAT_R16G16B16A16_UNORM:    return 4;
	case DXGI_FORMAT_R16G16B16A16_UINT:     return 4;
	case DXGI_FORMAT_R16G16B16A16_SNORM:    return 4;
	case DXGI_FORMAT_R16G16B16A16_SINT:     return 4;
	case DXGI_FORMAT_R32G32_TYPELESS:       return 2;
	case DXGI_FORMAT_R32G32_FLOAT:          return 2;
	case DXGI_FORMAT_R32G32_UINT:           return 2;
	case DXGI_FORMAT_R32G32_SINT:           return 2;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM:        return 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return 4;
	case DXGI_FORMAT_R8G8B8A8_UINT:         return 4;
	case DXGI_FORMAT_R8G8B8A8_SNORM:        return 4;
	case DXGI_FORMAT_R8G8B8A8_SINT:         return 4;
	case DXGI_FORMAT_R16G16_TYPELESS:       return 2;
	case DXGI_FORMAT_R16G16_FLOAT:          return 2;
	case DXGI_FORMAT_R16G16_UNORM:          return 2;
	case DXGI_FORMAT_R16G16_UINT:           return 2;
	case DXGI_FORMAT_R16G16_SNORM:          return 2;
	case DXGI_FORMAT_R16G16_SINT:           return 2;
	case DXGI_FORMAT_R32_TYPELESS:          return 1;
	case DXGI_FORMAT_D32_FLOAT:             return 1;
	case DXGI_FORMAT_R32_FLOAT:             return 1;
	case DXGI_FORMAT_R32_UINT:              return 1;
	case DXGI_FORMAT_R32_SINT:              return 1;
	case DXGI_FORMAT_R8G8_TYPELESS:         return 2;
	case DXGI_FORMAT_R8G8_UNORM:            return 2;
	case DXGI_FORMAT_R8G8_UINT:             return 2;
	case DXGI_FORMAT_R8G8_SNORM:            return 2;
	case DXGI_FORMAT_R8G8_SINT:             return 2;
	case DXGI_FORMAT_R16_TYPELESS:          return 1;
	case DXGI_FORMAT_R16_FLOAT:             return 1;
	case DXGI_FORMAT_D16_UNORM:             return 1;
	case DXGI_FORMAT_R16_UNORM:             return 1;
	case DXGI_FORMAT_R16_UINT:              return 1;
	case DXGI_FORMAT_R16_SNORM:             return 1;
	case DXGI_FORMAT_R16_SINT:              return 1;
	case DXGI_FORMAT_R8_TYPELESS:           return 1;
	case DXGI_FORMAT_R8_UNORM:              return 1;
	case DXGI_FORMAT_R8_UINT:               return 1;
	case DXGI_FORMAT_R8_SNORM:              return 1;
	case DXGI_FORMAT_R8_SINT:               return 1;
	default:                                return 0;
	}
}
