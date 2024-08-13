#include "texture_demo.h"
#include "math/vector.h"
#include "utility/shader_compiler.h"

TextureDemo::TextureDemo()
	: Application(graphics::GraphicsAPI::VULKAN)
{
	utility::ShaderCompiler::CompileShaders("shader/", "shader/bin/");
	Window::Resize(1600, 90);
	Application::Resize(2560, 1440);

	graphics::Texture::Layout textureLayout;
	textureLayout.imagePath_ = "wow.png";
	textureLayout.stage_ = graphics::ShaderBinding::Stage::PIXEL;
	textureLayout.slot_ = 0;
	texture_ = graphicsAPI_->CreateTexture(textureLayout);

	graphics::Mesh::Layout planeLayout;
	utility::ByteBuffer::Layout vertexLayout;
	vertexLayout.AddAttribute<math::Float2>();
	vertexLayout.AddAttribute<math::Float2>();
	planeLayout.vertices_.SetLayout(vertexLayout);

	auto vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(-1.0f, -1.0f);
	vertex.Get<math::Float2>(1) = math::Float2(0.0f, 0.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(1.0f,-1.0f);
	vertex.Get<math::Float2>(1) = math::Float2(1.0f, 0.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(1.0f, 1.0f);
	vertex.Get<math::Float2>(1) = math::Float2(1.0f, 1.0f);
	vertex = planeLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(-1.0f, 1.0f);
	vertex.Get<math::Float2>(1) = math::Float2(0.0f, 1.0f);
	planeLayout.indices_ = { 0, 1, 2, 2, 3, 0 };
	plane_ = graphicsAPI_->CreateMesh(planeLayout);

	graphics::ShaderDescriptor::Output colorOutput{};
	colorOutput.width_ = 1600;
	colorOutput.height_ = 900;
	colorOutput.format_ = graphics::ImageFormat::B8G8R8A8_UNORM;
	colorOutput.usage_ = graphics::ImageUsage::COLOR_ATTACHMENT;
	colorOutput.loadOp_ = graphics::ImageOperation::CLEAR;
	colorOutput.storeOp_ = graphics::ImageOperation::STORE;
	graphics::ShaderDescriptor::Output depthOutput = colorOutput;
	depthOutput.format_ = graphics::ImageFormat::D32_SFLOAT_U8_UINT;
	depthOutput.usage_ = graphics::ImageUsage::DEPTH_STENCIL;

	graphics::Pipeline::Layout pipelineLayout;
	pipelineLayout.primitiveTopology_ = graphics::PrimitiveTopology::TRIANGLE_LIST;
	pipelineLayout.vertexShaderPath_ = L"shader/bin/texture.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"shader/bin/texture.frag.spv";
	pipelineLayout.vertexInputLayout_ = vertexLayout;
	pipelineLayout.depthFunc_ = graphics::ComparisonFunc::LESS_EQUAL;
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