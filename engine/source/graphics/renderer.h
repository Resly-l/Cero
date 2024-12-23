#pragma once
#include "graphics/graphics_api.h"
#include <unordered_map>

namespace graphics
{
	class Renderer
	{
	private:
		std::shared_ptr<RenderPass> entryPass_;
		PassResources passResources_;

	public:
		void SetEntryPass(std::shared_ptr<RenderPass> _renderPass);
		void RenderFrame(GraphicsAPI& _graphicsAPI);
	};
}