#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>
#include <format>

int main()
{
	constexpr int numMaxTestCase = 41;
	int fiboTable[numMaxTestCase][2] = {};

	fiboTable[0][0] = 1;
	fiboTable[0][1] = 0;
	fiboTable[1][0] = 0;
	fiboTable[1][1] = 1;

	for (int i = 2; i < numMaxTestCase; i++)
	{
		fiboTable[i][0] = fiboTable[i - 1][0] + fiboTable[i - 2][0];
		fiboTable[i][1] = fiboTable[i - 1][1] + fiboTable[i - 2][1];
	}

	std::vector<std::pair<int, int>> answers;

	int numTestCase = 0;
	std::cin >> numTestCase;

	for (int i = 0; i < numTestCase; i++)
	{
		int fiboIndex = 0;
		std::cin >> fiboIndex;

		answers.push_back(std::make_pair(fiboTable[fiboIndex][0], fiboTable[fiboIndex][1]));
	}

	for (const auto& answer : answers)
	{
		std::cout << answer.first << " " << answer.second << std::endl;
	}

	return 0;
}