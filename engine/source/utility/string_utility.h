#pragma once
#include <string>

namespace utility
{
	std::wstring ToWide(std::string_view _string);
	std::string ToNarrow(std::wstring_view _string);
}