#include "hello_triangle.h"

//#define TRY_CATCH

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
#ifdef TRY_CATCH
	try
	{
		HelloTriangle{}.Run();
	}
	catch (const std::exception& _exception)
	{
		return 1;
	}
#else
	HelloTriangle{}.Run();
#endif
	return 0;
}