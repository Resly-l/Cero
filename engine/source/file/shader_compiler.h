#pragma once
#include <string_view>
#include "path.generated.h"

namespace file
{
	class ShaderCompiler final
	{
	public:
		static void CompileShaders(std::string_view _inputDirectory, std::string_view _outputDirectory)
		{
			system((std::string(PATH_SHADER_COMPILER) + " " + std::string(_inputDirectory) + " " + std::string(_outputDirectory)).c_str());
		}
	};
}