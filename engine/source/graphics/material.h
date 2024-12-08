#pragma once
#include <array>
#include <memory>
#include "utility/forward_declaration.h"

namespace graphics
{
	class Material
	{
	public:
		enum class FixedBindingIndex
		{
			DIFFUSE_MAP,
			NORMAL_MAP,
			FB_MAX
		};

	protected:
		std::array<std::shared_ptr<ShaderBinding>, (int32_t)FixedBindingIndex::FB_MAX> fixedBindings_;
		bool pendingCompilation_ = false;

	public:
		virtual ~Material() {}

	public:
		void SetFixedBinding(FixedBindingIndex _index, std::shared_ptr<ShaderBinding> _binding)
		{
			fixedBindings_[(uint32_t)_index] = _binding;
			pendingCompilation_ = true;
		}

		bool IsCompiled() const
		{
			return !pendingCompilation_;
		}
	};
}