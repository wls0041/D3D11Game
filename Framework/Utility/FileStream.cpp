#include "Framework.h"
#include "FileStream.h"

FileStream::FileStream(const std::string & path, const uint & flags)
{
	this->is_open = false;
	this->flags = flags;

	int ios_flags = std::ios::binary;
	ios_flags |= (this->flags & Stream_Read) ? std::ios::in : 0U;
	ios_flags |= (this->flags & Stream_Write) ? std::ios::out : 0U;
	ios_flags |= (this->flags & Stream_Append) ? std::ios::app : 0U;

	if (this->flags & Stream_Write)
	{
		out.open(path, ios_flags);
		if (out.fail())
		{
			LOG_ERROR_F("Failed to open \"%s\" for writing", path.c_str());
			return;
		}
	}
	else if (this->flags & Stream_Read)
	{
		in.open(path, ios_flags);
		if (in.fail())
		{
			LOG_ERROR_F("Failed to open \"%s\" for reading", path.c_str());
			return;
		}
	}

	is_open = true;
}

FileStream::~FileStream()
{
	Close();
}

void FileStream::Close()
{
	if (this->flags & Stream_Write)
	{
		out.flush();
		out.close();
	}
	else if (this->flags & Stream_Read)
	{
		in.clear();
		in.close();
	}
}

void FileStream::Write(const void * value, const uint & size)
{
	out.write(reinterpret_cast<const char*>(value), size);
}

void FileStream::Write(const std::string & value)
{
	const auto length = static_cast<uint>(value.length());
	Write(length);

	out.write(value.c_str(), length);
}

void FileStream::Write(const std::vector<std::byte>& value)
{
	const auto length = static_cast<uint>(value.size());
	Write(length);

	out.write(reinterpret_cast<const char*>(value.data()), length);
}

void FileStream::Skip(const uint & n)
{
	if (this->flags & Stream_Write)
		out.seekp(n, std::ios::cur);
	else if (this->flags & Stream_Read)
		in.ignore(n, std::ios::cur);
}

void FileStream::Read(std::string & value)
{
	auto length = Read<uint>();

	value.clear();
	value.shrink_to_fit();

	value.resize(length);
	in.read(value.data(), length);// == const_cast<char*>(value.c_str())
}

void FileStream::Read(std::vector<std::byte>& value)
{
	auto length = Read<uint>();

	value.clear();
	value.shrink_to_fit();

	value.reserve(length);
	value.resize(length);

	in.read(reinterpret_cast<char*>(value.data()), length);
}
