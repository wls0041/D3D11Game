#include "Framework.h"
#include "FileSystem.h"
#include <filesystem>

using namespace std::filesystem;

std::vector<std::string> FileSystem::support_texture_formats
{
	".jpg",
	".png",
	".bmp",
	".tga",
	".dds",
	".exr",
	".raw",
	".gif",
	".hdr",
	".ico",
	".iff",
	".jng",
	".jpeg",
	".koala",
	".kodak",
	".mng",
	".pcx",
	".pbm",
	".pgm",
	".ppm",
	".pfm",
	".pict",
	".psd",
	".raw",
	".sgi",
	".targa",
	".tiff",
	".tif",
	".wbmp",
	".webp",
	".xbm",
	".xpm"
};

std::vector<std::string> FileSystem::support_model_formats
{
	".3ds",
	".obj",
	".fbx",
	".blend",
	".dae",
	".lwo",
	".c4d",
	".ase",
	".dxf",
	".hmp",
	".md2",
	".md3",
	".md5",
	".mdc",
	".mdl",
	".nff",
	".ply",
	".stl",
	".x",
	".smd",
	".lxo",
	".lws",
	".ter",
	".ac3d",
	".ms3d",
	".cob",
	".q3bsp",
	".xgl",
	".csm",
	".bvh",
	".b3d",
	".ndo"
};

auto FileSystem::Create_Directory(const std::string & path) -> const bool
{
	try
	{
		return create_directories(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_Directory(const std::string & path) -> const bool
{
	try
	{
		return remove_all(path) > 0; //���� ������ ������ ����
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_File(const std::string & path) -> const bool
{
	if (is_directory(path)) return false;//file���� Ȯ��

	try
	{
		return remove(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Copy_File(const std::string & src, const std::string & dst) -> const bool
{
	if (src == dst)
		return false;

	if (!IsExistDirectory(GetDirectoryFromPath(dst)))
		Create_Directory(GetDirectoryFromPath(dst));

	try
	{
		return copy_file(src, dst, copy_options::overwrite_existing);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR("Could not copy \"" + src + "\", " + std::string(error.what()));
		return false;
	}
}

auto FileSystem::IsDirectory(const std::string & path) -> const bool
{
	try
	{
		return is_directory(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistDirectory(const std::string & path) -> const bool
{
	try
	{
		return exists(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistFile(const std::string & path) -> const bool
{
	try
	{
		return exists(path);
	}
	catch (filesystem_error& error)
	{
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::GetFileNameFromPath(const std::string & path) -> const std::string
{
	auto last_index = path.find_last_of("\\/");
	auto file_name = path.substr(last_index + 1, path.length());

	return file_name;
}

auto FileSystem::GetIntactFileNameFromPath(const std::string & path) -> const std::string
{
	auto file_name = GetFileNameFromPath(path);
	auto last_index = file_name.find_last_of(".");
	auto intact_file_name = file_name.substr(0, last_index);

	return intact_file_name;
}

auto FileSystem::GetDirectoryFromPath(const std::string & path) -> const std::string
{
	auto last_index = path.find_last_of("\\/");
	auto directory = path.substr(0, last_index + 1);

	return directory;
}

auto FileSystem::GetExtensionFromPath(const std::string & path) -> const std::string
{
	if (path.empty() || path == NOT_ASSIGNED_STR)
		return NOT_ASSIGNED_STR;

	auto last_index = path.find_last_of('.');
	if (last_index != std::string::npos)
		return path.substr(last_index, path.length());

	return NOT_ASSIGNED_STR;
}

auto FileSystem::GetPathWithoutExtension(const std::string & path) -> const std::string
{
	auto directory = GetDirectoryFromPath(path);
	auto intact_file_name = GetIntactFileNameFromPath(path);

	return directory + intact_file_name;
}

auto FileSystem::GetRelativeFromPath(const std::string & absolute_path) -> const std::string
{
	//���� ��θ� ����
	path p = absolute(absolute_path);
	path r = absolute(GetWorkingDirectory());

	//��Ʈ ��ΰ� �ٸ���� ������ ��ȯ
	if (p.root_path() != r.root_path())
		return p.generic_string();

	path result;

	//�� ��ΰ� �������� ������ üũ
	path::const_iterator iter_path = p.begin();
	path::const_iterator iter_relative = r.begin();

	while (
		*iter_path == *iter_relative &&
		iter_path != p.end() &&
		iter_relative != r.end())
	{
		iter_path++;
		iter_relative++;
	}

	//relative�� ���� �� ��ū������ ..�� �߰�
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			result /= "..";
			iter_relative++;
		}
	}

	//���� ��� �߰�
	while (iter_path != p.end())
	{
		result /= *iter_path;
		iter_path++;
	}

	return result.generic_string();
}

auto FileSystem::GetParentDirectory(const std::string & path) -> const std::string
{
	auto found = path.find_last_of("\\/");
	auto result = path;

	if (found == std::string::npos)
		return path;

	if (found == path.length() - 1)
	{
		result = result.substr(0, found - 1);
		return GetParentDirectory(result);
	}

	return result.substr(0, found) + "/";
}

auto FileSystem::GetWorkingDirectory() -> const std::string
{
	return current_path().generic_string() + "/";
}

auto FileSystem::IsSupportTextureFile(const std::string & path) -> const bool
{
	auto file_extension = GetExtensionFromPath(path);

	auto support_formats = GetSupportTextureFormats();
	for (const auto& format : support_formats)
	{
		if (file_extension == format || file_extension == ToUpper(format))
			return true;
	}
	return false;
}

auto FileSystem::IsSupportModelFile(const std::string & path) -> const bool
{
	auto file_extension = GetExtensionFromPath(path);

	auto support_formats = GetSupportModelFormats();
	for (const auto& format : support_formats)
	{
		if (file_extension == format || file_extension == ToUpper(format))
			return true;
	}
	return false;
}

auto FileSystem::ToUpper(const std::string & lower) -> const std::string
{
	std::string upper;

	for (const auto& character : lower)
		upper += toupper(character);

	return upper;
}

auto FileSystem::ToLower(const std::string & upper) -> const std::string
{
	std::string lower;

	for (const auto& character : upper)
		lower += tolower(character);

	return lower;
}

auto FileSystem::ToString(const std::wstring & str) -> const std::string
{
	//����
	return std::string();
}

auto FileSystem::ToWstring(const std::string & str) -> const std::wstring
{
	std::wstring result;
	result.assign(str.begin(), str.end());

	return result;
}

