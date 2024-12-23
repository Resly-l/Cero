#include "render_pass.h"
#include "utility/log.h"
#include <functional>

using utility::Log;

namespace graphics
{
	void RenderPass::AddDependent(std::shared_ptr<RenderPass> _dependent)
	{
		std::function<bool(std::vector<std::shared_ptr<RenderPass>>)> IsIncludedInDependency;
		IsIncludedInDependency = [_dependent, &IsIncludedInDependency](const auto& _dependencies) -> bool
			{
				for (const auto& dependency : _dependencies)
				{
					IsIncludedInDependency(dependency->dependencies_);

					if (dependency == _dependent)
					{
						return true;
					}
				}

				return false;
			};

		if (IsIncludedInDependency(dependencies_))
		{
			std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "Tried to add dependent which was included in dependencies") << std::endl;
			return;
		}

		for (const auto& dependent : dependents_)
		{
			if (dependent == _dependent)
			{
				std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "Duplicate AddDependent() call") << std::endl;
				return;
			}
		}

		dependents_.push_back(_dependent);
		_dependent->dependencies_.push_back(shared_from_this());
	}
}