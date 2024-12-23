#pragma once
#include "common.h"
#include <memory>
#include <vector>

namespace graphics
{
	class RenderPass : public std::enable_shared_from_this<RenderPass>
	{
	private:
		// deal with theses after seperation
		std::vector<std::shared_ptr<RenderPass>> dependencies_;
		std::vector<std::shared_ptr<RenderPass>> dependents_;

	protected:
		std::shared_ptr<Pipeline> pipeline_;

	public:
		virtual ~RenderPass() = default;

	public:
		void AddDependent(std::shared_ptr<RenderPass> _dependent);

		virtual void SetPipeline(std::shared_ptr<Pipeline> _pipeline, GraphicsAPI& _graphicsAPI) = 0;
		virtual void AddDrawable(Drawable _drawable) = 0;
		virtual void Execute(GraphicsAPI& _graphicsApi, PassResources& _resources) = 0;
	};
}