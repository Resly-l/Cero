#include "hello_triangle.h"

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
		std::string exceptionDescription = _exception.what();
		MessageBox(nullptr, std::wstring(exceptionDescription.begin(), exceptionDescription.end()).c_str(), L"exception", MB_OK);
		return 1;
	}
#else
	HelloTriangle{}.Run();
#endif
	return 0;
}