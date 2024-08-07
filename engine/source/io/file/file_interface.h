#pragma once
#include <string_view>
#include <fstream>

namespace io::file
{
	class FileInterface final
	{
	public:
		static std::vector<char> LoadFile(std::wstring_view _filePath, bool _reversed, bool _binary)
		{
			std::vector<char> buffer;
			std::ifstream file(_filePath.data(), (_reversed ? std::ios::ate : 0) | (_binary ? std::ios::binary : 0));

			if (!file.is_open())
			{
				return buffer;
			}

			buffer.resize((size_t)file.tellg());

			file.seekg(0);
			file.read(buffer.data(), buffer.size());

			file.close();
			return buffer;
		}
	};
}