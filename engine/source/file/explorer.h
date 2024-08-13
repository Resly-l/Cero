#pragma once
#include <string_view>
#include <fstream>
#include <vector>

namespace file
{
	class Explorer final
	{
	public:
		static std::vector<char> LoadFile(std::wstring_view _filePath, bool _reversed, bool _binary);
	};
}