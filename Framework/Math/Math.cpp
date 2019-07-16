#include "Math.h"
#include <random>

std::random_device rd;
std::mt19937 randomEngine(rd()); //�޵弾 Ʈ������

auto Math::Random(const int & min, const int & max) -> const int
{
	return std::uniform_int_distribution<int> {min, max}(randomEngine);
}

auto Math::Random(const float & min, const float & max) -> const float
{
	//Uniform Initialization - �߰�ȣ �ʱ�ȭ. c++11���� �߰�, ������ ���ÿ� �ʱ�ȭ ��Ŵ, �����̸� �ʿ���� �ٷ� �ʱ�ȭ ����.
	return std::uniform_real_distribution<float> {min, max}(randomEngine);  //���Ժ���

	//����
	//std::uniform_real_distribution<float> test(min, max);
	//float random = test(randomEngine);
}
