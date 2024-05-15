#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>

int main()
{
	int ary[5] = { 1,2,3,4,5 };

	for (int i = 0; i < 5; i++)
	{
		std::cout << i[ary] << std::endl;
	}

	return 0;
}