#pragma once

//[GUID, UUID]
//universally->범용적인.여타 다른 운영체제에서도 쓰기 위해 만들어짐. 얘도 16byte
//GUID -> 16byte짜리. ms에서 따로만들긴 했는데 별 차이는 없음

//16byte 그대로 쓰기에는 너무 커서 부담됨. -> 이를 줄여서 사용할 예정
class GUID_Generator final
{
public:
	static auto Generate() -> const uint;
	static auto GenerateToString() -> const std::string;
};