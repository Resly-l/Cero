#pragma once

namespace window
{
	class Window;
	class Application;
}

namespace graphics
{
	class GraphicsAPI;
	class Pipeline;
	class RenderTarget;
	class UniformBuffer;
	class Mesh;
	class Material;
	class Model;
	class Texture;

	//struct ShaderBinding;
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
	class Explorer;
	struct Image;
	struct Model;
}