#include "Window/Window.h"
#include <iostream>

int main()
{
	try
	{
		Window{ 1300, 900, "Hello Triangle" }.Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}