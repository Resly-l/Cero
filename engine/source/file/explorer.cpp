#include "explorer.h"

namespace file
{
	std::vector<char> Explorer::LoadFile(std::wstring_view _filePath, bool _reversed, bool _binary)
	{
		std::ifstream file(_filePath.data(), (_reversed ? std::ios::ate : 0) | (_binary ? std::ios::binary : 0));
		if (!file.is_open())
		{
			return std::vector<char>{};
		}

		std::vector<char> buffer((size_t)file.tellg());
		file.seekg(0);
		file.read(buffer.data(), buffer.size());

		file.close();
		return buffer;
	}
}