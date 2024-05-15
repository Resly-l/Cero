#include "hello_triangle.h"
#include "utility/string_utility.h"

#define TRY_CATCH 1

int main()
{
#if TRY_CATCH
	try
	{
		HelloTriangle{}.Run();
	}
	catch (const std::exception& _exception)
	{
		MessageBox(nullptr, utility::ToWide(_exception.what()).c_str(), L"Exception occured", MB_OK);
		return 1;
	}
#else
	HelloTriangle{}.Run();
#endif
	return 0;
}