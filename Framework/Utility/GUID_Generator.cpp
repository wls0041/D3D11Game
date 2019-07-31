#include "Framework.h"
#include "GUID_Generator.h"
#include <sstream>
#include <iomanip> //입출력장치 세밀하게 조정
#include <guiddef.h>

auto GUID_Generator::Generate() -> const uint
{
	std::hash<std::string> hasher;

	return static_cast<uint>(hasher(GenerateToString()));
}

auto GUID_Generator::GenerateToString() -> const std::string
{
	GUID guid; // --> 4byte, 2byte, 2byte, 8byte. 총 16byte로 쪼개져 있음
	auto hr = CoCreateGuid(&guid);
	if (SUCCEEDED(hr))
	{
		std::stringstream sstream;
		sstream << std::hex << std::uppercase
				<< std::setw(8) << std::setfill('0') << guid.Data1 << "-" //haxa를 쓸 것.  16진수 표기법이기 때문에 8로 표현 
				<< std::setw(4) << std::setfill('0') << guid.Data2 << "-"
				<< std::setw(4) << std::setfill('0') << guid.Data3 << "-"
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[0]) //원본이 문자형인데 정수로 쓰고 싶어서 int로 변환
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[1])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[2])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[3])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[4])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[5])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[6])
				<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[7]);

		return sstream.str();
	}

	return NOT_ASSIGNED_STR;
}
