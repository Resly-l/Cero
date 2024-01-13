#pragma once
#include "pipeline_state.h"

namespace io::graphics
{
	class GraphicsDevice
	{

	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const GraphicsDevice& _device, const PipelineState& _pipelineState) {};
		virtual ~GraphicsPipeline() = default;
	};

	class GraphicsAPI
	{
	protected:
		std::unique_ptr<GraphicsDevice> device_;
		std::unique_ptr<GraphicsPipeline> pipeline_;

	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual void BindPipeline(const PipelineState& _pipelineState) = 0;
		virtual void Execute() const = 0;

	protected:
		template <typename T>
		T* GetDevice() const
		{
			return reinterpret_cast<T*>(device_.get());
		}

		template  <typename T>
		T* GetPipeline() const
		{
			return reinterpret_cast<T*>(pipeline_.get());
		}
	};
}