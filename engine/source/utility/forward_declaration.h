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
		class Pipeline;
		class RenderTarget;
		class Mesh;
		class UniformBuffer;
		class Texture;

		class ShaderBinding;
		struct ShaderDescriptor;

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
		class VulkanShaderBinding;
		class VulkanUniformBuffer;
		class VulkanTexture;
	}

	namespace file
	{
		class FileInterface;
		struct Image;
	}
}