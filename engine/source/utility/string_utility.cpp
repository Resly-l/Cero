#include "string_utility.h"

namespace utility
{
	std::wstring ToWide(std::string_view _string)
	{
		return std::wstring(_string.begin(), _string.end());
	}

	std::string ToNarrow(std::wstring_view _string)
	{
		return std::string(_string.begin(), _string.end());
	}
}