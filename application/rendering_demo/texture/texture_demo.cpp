#include "texture_demo.h"
#include "core/math/vector.h"
#include "io/file/shader_compiler.h"

TextureDemo::TextureDemo()
	: Application(io::graphics::GraphicsAPI::VULKAN)
{
	io::file::ShaderCompiler::CompileShaders("shader/", "shader/bin/");
	Window::Resize(1600, 90);
	Application::Resize(2560, 1440);

	io::graphics::Texture::Layout textureLayout;
	textureLayout.imagePath_ = "wow.png";
	textureLayout.stage_ = io::graphics::ShaderBinding::Stage::PIXEL;
	textureLayout.slot_ = 0;
	texture_ = graphicsAPI_->CreateTexture(textureLayout);

	io::graphics::Mesh::Layout planeLayout;
	utility::ByteBuffer::Layout vertexLayout;
	vertexLayout.AddAttribute<math::Vector2d<float>>();
	vertexLayout.AddAttribute<math::Vector2d<float>>();
	planeLayout.vertices_.SetLayout(vertexLayout);

	auto vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d<float>(-1.0f, -1.0f);
	vertex.Get<math::Vector2d<float>>(1) = math::Vector2d<float>(0.0f, 0.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d<float>(1.0f,-1.0f);
	vertex.Get<math::Vector2d<float>>(1) = math::Vector2d<float>(1.0f, 0.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d<float>(1.0f, 1.0f);
	vertex.Get<math::Vector2d<float>>(1) = math::Vector2d<float>(1.0f, 1.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d<float>(-1.0f, 1.0f);
	vertex.Get<math::Vector2d<float>>(1) = math::Vector2d<float>(0.0f, 1.0f);
	planeLayout.indices_ = { 0, 1, 2, 2, 3, 0 };
	plane_ = graphicsAPI_->CreateMesh(planeLayout);

	io::graphics::ShaderDescriptor::Output colorOutput{};
	colorOutput.width_ = 1600;
	colorOutput.height_ = 900;
	colorOutput.format_ = io::graphics::ImageFormat::B8G8R8A8_UNORM;
	colorOutput.usage_ = io::graphics::ImageUsage::COLOR_ATTACHMENT;
	colorOutput.loadOp_ = io::graphics::ImageOperation::CLEAR;
	colorOutput.storeOp_ = io::graphics::ImageOperation::STORE;
	io::graphics::ShaderDescriptor::Output depthOutput = colorOutput;
	depthOutput.format_ = io::graphics::ImageFormat::D32_SFLOAT_U8_UINT;
	depthOutput.usage_ = io::graphics::ImageUsage::DEPTH_STENCIL;

	io::graphics::Pipeline::Layout pipelineLayout;
	pipelineLayout.primitiveTopology_ = io::graphics::PrimitiveTopology::TRIANGLE_LIST;
	pipelineLayout.vertexShaderPath_ = L"shader/bin/texture.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"shader/bin/texture.frag.spv";
	pipelineLayout.vertexInputLayout_ = vertexLayout;
	pipelineLayout.depthFunc_ = io::graphics::ComparisonFunc::LESS_EQUAL;
	pipelineLayout.descriptor_.resources_.push_back(texture_);
	pipelineLayout.descriptor_.outputs.push_back(colorOutput);
	pipelineLayout.descriptor_.outputs.push_back(depthOutput);
	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);
}

void TextureDemo::Update()
{

}

void TextureDemo::Render()
{
	graphicsAPI_->BindPipeline(pipeline_);
	graphicsAPI_->BindMesh(plane_);
	graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->Draw();
}