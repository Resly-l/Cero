#include "hello_triangle.h"

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
	try
	{
		HelloTriangle{}.Run();
	}
	catch (const std::exception& _exception)
	{
		return 1;
	}
	return 0;
}