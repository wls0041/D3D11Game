#pragma once

enum StreamMode : uint
{
	Stream_Read = 1U << 0,
	Stream_Write = 1U << 1,
	Stream_Append = 1U << 2,
};

class FileStream final // -> file들을 binary 파일로 저장해놓고 불러오거나 내보내는 클래스
{
public:
	FileStream(const std::string & path, const StreamMode & mode);
	~FileStream();

	auto IsOpen() const -> const bool { return is_open; }
	void Close();

	////////////////////////Write////////////////////////
	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value >::type>
	void Write(const T &value) 
	{
		out.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	void Write(const std::string &value);
	void Write(const std::vector<std::byte> &value);

	void Skip(const uint &n);


	////////////////////////Read/////////////////////////	
	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value > ::type >
	void Read(T &value) //얘는 읽어온 파일을 value에 넣으므로 const 붙으면 안됨
	{
		in.write(reinterpret_cast<char*>(&value), sizeof(T));
	}

	void Read(std::string &value);
	void Read(std::vector<std::byte> &value);

	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value > ::type >
	const T Read()
	{
		T value;
		Read(value);

		return value;
	}

private:
	std::ofstream out;
	std::ifstream in;

	uint flags;
	bool is_open;
};