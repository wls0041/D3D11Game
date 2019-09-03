#pragma once

class FileSystem final
{
public:
	static void OpenDirectoryWindow(const std::string &directory);

	static auto Create_Directory(const std::string& path) -> const bool;
	static auto Delete_Directory(const std::string& path) -> const bool;
	static auto Delete_File(const std::string& path) -> const bool;
	static auto Copy_File(const std::string& src, const std::string& dst) -> const bool;
	static auto IsDirectory(const std::string& path) -> const bool;
	static auto IsExistDirectory(const std::string& path) -> const bool;
	static auto IsExistFile(const std::string& path) -> const bool;

	static auto GetFileNameFromPath(const std::string& path) -> const std::string;
	static auto GetIntactFileNameFromPath(const std::string& path) -> const std::string;
	static auto GetDirectoryFromPath(const std::string& path) -> const std::string;
	static auto GetExtensionFromPath(const std::string& path) -> const std::string;
	static auto GetPathWithoutExtension(const std::string& path) -> const std::string;
	static auto GetRelativeFromPath(const std::string& absolute_path) -> const std::string;
	static auto GetParentDirectory(const std::string& path) -> const std::string;
	static auto GetWorkingDirectory() -> const std::string;

	static auto GetDirectoriesInDirectory(const std::string& directory) -> const std::vector<std::string>;
	static auto GetFilesInDirectory(const std::string& directory) -> const std::vector<std::string>;
	static auto GetSupportModelFilesInDirectory(const std::string &directory) -> const std::vector<std::string>;
	static auto GetSupportTextureFilesInDirectory(const std::string &directory) -> const std::vector<std::string>;

	static auto GetSupportTextureFormats() -> const std::vector<std::string>& { return support_texture_formats; }
	static auto GetSupportModelFormats() -> const std::vector<std::string>& { return support_model_formats; }
	static auto GetSupportScriptFormats() -> const std::vector<std::string>& { return support_script_formats; }

	static auto GetSupportModelFilesFromPaths(const std::vector<std::string> &paths) -> const std::vector<std::string>;
	static auto GetSupportTextureFilesFromPaths(const std::vector<std::string> &paths) -> const std::vector<std::string>;

	static auto IsSupportTextureFile(const std::string& path) -> const bool;
	static auto IsSupportModelFile(const std::string& path) -> const bool;
	static auto IsSupportScriptFile(const std::string& path) -> const bool;

	static auto ToUpper(const std::string& lower) -> const std::string;
	static auto ToLower(const std::string& upper) -> const std::string;
	static auto ToString(const std::wstring& str) -> const std::string;
	static auto ToWstring(const std::string& str) -> const std::wstring;

private:
	static std::vector<std::string> support_texture_formats;
	static std::vector<std::string> support_model_formats;
	static std::vector<std::string> support_script_formats;
};