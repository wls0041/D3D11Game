#pragma once

//[GUID, UUID]
//universally->��������.��Ÿ �ٸ� �ü�������� ���� ���� �������. �굵 16byte
//GUID -> 16byte¥��. ms���� ���θ���� �ߴµ� �� ���̴� ����

//16byte �״�� ���⿡�� �ʹ� Ŀ�� �δ��. -> �̸� �ٿ��� ����� ����
class GUID_Generator final
{
public:
	static auto Generate() -> const uint;
	static auto GenerateToString() -> const std::string;
};