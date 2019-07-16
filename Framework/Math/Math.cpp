#include "Math.h"
#include <random>

std::random_device rd;
std::mt19937 randomEngine(rd()); //메드센 트리스터

auto Math::Random(const int & min, const int & max) -> const int
{
	return std::uniform_int_distribution<int> {min, max}(randomEngine);
}

auto Math::Random(const float & min, const float & max) -> const float
{
	//Uniform Initialization - 중괄호 초기화. c++11부터 추가, 생성과 동시에 초기화 시킴, 변수이름 필요없이 바로 초기화 가능.
	return std::uniform_real_distribution<float> {min, max}(randomEngine);  //정규분포

	//정석
	//std::uniform_real_distribution<float> test(min, max);
	//float random = test(randomEngine);
}
