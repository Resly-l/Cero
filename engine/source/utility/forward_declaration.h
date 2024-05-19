#pragma once

namespace io
{
	namespace window
	{
		class Window;
	}

	namespace graphics
	{
		class GraphicsAPI;
		class Framebuffer;
		class Pipeline;
		class RenderTarget;
		class Mesh;

		struct Viewport;
		enum class PrimitiveTopology;
		enum class ComparisonFunc;
		enum class RasterizerState;
		enum class BlendState;
		enum class ImageFormat;
		enum class ImageUsage;

		class VulkanAPI;
		class VulkanPipeline;
		class VulkanRenderTarget;
		class VulkanMesh;
	}

	namespace file
	{
		class FileInterface;
	}
}