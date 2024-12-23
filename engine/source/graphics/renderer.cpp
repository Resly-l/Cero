#include "renderer.h"
#include "render_pass.h"
#include "utility/log.h"

using utility::Log;

namespace graphics
{
	void Renderer::SetEntryPass(std::shared_ptr<RenderPass> _renderPass)
	{
		entryPass_ = _renderPass;
	}

	void Renderer::RenderFrame(GraphicsAPI& _graphicsAPI)
	{
		if (!entryPass_)
		{
			std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "Renderer::RenderFrame() - entryPass_ was not set") << std::endl;
			return;
		}

		entryPass_->Execute(_graphicsAPI, passResources_);
	}
}