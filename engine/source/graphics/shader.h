#pragma once
#include "graphics/common.h"
#include <vector>
#include <memory>

namespace graphics
{
	class ShaderBinding
	{
	public:
		enum Stage : uint32_t
		{
			VERTEX = 1 << 0,
			PIXEL = 1 << 1,
		};

	public:
		virtual ~ShaderBinding() {}
	};

	class ShaderResource
	{
	protected:
		uint32_t slot_ = 0;
		uint32_t numElements_ = 0;

	public:
		virtual ~ShaderResource() {}

		virtual bool IsPendingUpdate() const { return false; }
		virtual void Update() {}
		virtual std::shared_ptr<ShaderBinding> GetShaderBinding() const = 0;
	};

	struct ShaderDescriptor
	{
		struct Output
		{
			ImageFormat format_;
			ImageUsage usage_;
			ImageOperation loadOp_;
			ImageOperation storeOp_;
			uint32_t width_;
			uint32_t height_;
		};

		std::vector<std::shared_ptr<ShaderResource>> resources_;
		std::vector<Output> outputs;
	};
}