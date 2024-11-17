#pragma once

namespace graphics
{
	class Material
	{
	public:
		struct Layout
		{
			file::Material materiallFile_;
			std::vector<ShaderBinding> bindings_;
		};
	public:
		virtual ~Material() {}
	};
}