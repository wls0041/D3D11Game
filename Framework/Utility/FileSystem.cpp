#include "Framework.h"
#include "FileSystem.h"
#include <filesystem>

using namespace std::filesystem;

auto FileSystem::Create_Directory(const std::string & path) -> const bool
{
	try {
		return create_directories(path);
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_Directory(const std::string & path) -> const bool
{
	try {
		return remove_all(path) > 0; //지운 파일의 개수가 나옴
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Delete_File(const std::string & path) -> const bool
{
	if (is_directory(path)) return false; //file인지 확인

	try {
		return remove(path);
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::Copy_File(const std::string & src, const std::string & dst) -> const bool
{
	if (src == dst) return false;

	if (!IsExistDirectory(GetDirectoryFromPath(dst)))
		Create_Directory(GetDirectoryFromPath(dst));

	try {
		return copy_file(src, dst, copy_options::overwrite_existing);
	}
	catch (filesystem_error &error) {
		LOG_ERROR("Could not copy \"" + src + "\", " + std::string(error.what()));
		return false;
	}
}

auto FileSystem::IsDirectory(const std::string & path) -> const bool
{
	try {
		return is_directory(path);
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistDirectory(const std::string & path) -> const bool
{
	try {
		return exists(path);
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::IsExistFile(const std::string & path) -> const bool
{
	try {
		return remove(path);
	}
	catch (filesystem_error &error) {
		LOG_ERROR_F("%s, %s", error.what(), path.c_str());
		return false;
	}
}

auto FileSystem::GetFileNameFromPath(const std::string & path) -> const std::string
{
	auto last_index = path.find_last_of("\\/");
	auto file_name = path.substr(last_index + 1, path.length());

	return std::string();
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
	auto lastIndex = path.find_last_of("\\/");
	auto directory = path.substr(0, lastIndex + 1);

	return directory;
}

auto FileSystem::GetExtensionFromPath(const std::string & path) -> const std::string
{
	if (path.empty() || path == NOT_ASSIGNED_STR)
		return NOT_ASSIGNED_STR;

	auto lastIndex = path.find_last_of('.');

	if (lastIndex != std::string::npos)
		return path.substr(lastIndex, path.length());

	return NOT_ASSIGNED_STR;
}

auto FileSystem::GetPathWithoutExtension(const std::string & path) -> const std::string
{
	auto directory = GetDirectoryFromPath(path);
	auto intactFileName = GetIntactFileNameFromPath(path);

	return directory + intactFileName;
}

auto FileSystem::GetRelativeFromPath(const std::string & absolute_path) -> const std::string
{   
	//절대 경로를 만듬
	path p = absolute(absolute_path);
	path r = absolute(GetWorkingDirectory());

	//루트 경로가 다를경우 절대경로 반환
	if (p.root_path() != r.root_path())
		return p.generic_string();

	path result;

	//두 경로가 갈라지는 지점을 체크
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

	//relative에 남은 각 토큰에대해 ..을 추가
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			result /= "..";
			iter_relative++;
		}
	}

	//남은 경로 추가
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
	//과제
	return std::string();
}

auto FileSystem::ToWstring(const std::string & str) -> const std::wstring
{
	std::wstring result;
	result.assign(str.begin(), str.end());

	return result;
}
