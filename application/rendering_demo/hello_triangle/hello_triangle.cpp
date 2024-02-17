#include "hello_triangle.h"
#include "io/graphics/renderer.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"

HelloTriangle::HelloTriangle()
{
	Window::SetTitle("Hello Triangle");

	io::graphics::PipelineState pipelineState;

	pipelineState.vertexShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
	pipelineState.pixelShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";

	pipelineState.viewport_.width_ = 1600;
	pipelineState.viewport_.height_ = 900;

	utility::ByteBuffer::Layout vertexLayout;
	vertexLayout.AddAttribute<math::Vec2<float>>("position");
	vertexLayout.AddAttribute<math::Vec3<float>>("color");
	pipelineState.vertexBuffer.SetLayout(vertexLayout);

	if (auto vertex = pipelineState.vertexBuffer.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.0f, -0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
	}

	if (auto vertex = pipelineState.vertexBuffer.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.15f, 0.15f, 0.15f);
	}

	if (auto vertex = pipelineState.vertexBuffer.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.69f, 0.22f, 0.25f);
	}

	io::graphics::RenderPassLayout renderPassLayout;
	io::graphics::AttachmentDescription attachmentDescription{};
	attachmentDescription.format_ = io::graphics::ImageFormat::ifB8G8R8A8_UNORM;
	attachmentDescription.usage_ = io::graphics::ImageUsage::iuCOLOR_ATTACHMENT;
	attachmentDescription.loadOperation_ = io::graphics::ImageOperation::ioCLEAR;
	attachmentDescription.storeOperation_ = io::graphics::ImageOperation::ioSTORE;
	attachmentDescription.initialLayout_ = io::graphics::ImageLayout::ilUNDEFINED;
	attachmentDescription.finalLayout_ = io::graphics::ImageLayout::ilSHADER_READ;
	attachmentDescription.width_ = 1600;
	attachmentDescription.height_ = 900;
	renderPassLayout.attachmentDescriptions_.push_back(attachmentDescription);
	renderPassLayout.attachmentDescriptions_.push_back(attachmentDescription);
	renderPassLayout.attachmentDescriptions_.push_back(attachmentDescription);

	attachmentDescription.format_ = io::graphics::ImageFormat::ifD32_SFLOAT;
	attachmentDescription.usage_ = io::graphics::ImageUsage::iuDEPTH_STENCIL_ATTACHMENT;
	attachmentDescription.initialLayout_ = io::graphics::ImageLayout::ilUNDEFINED;
	attachmentDescription.finalLayout_ = io::graphics::ImageLayout::ilDEPTH_STENCIL;
	renderPassLayout.attachmentDescriptions_.push_back(attachmentDescription);

	io::graphics::SubpassDescription subpassDescription{};
	subpassDescription.numColorReferences_ = 3;
	renderPassLayout.subpassDescriptions_.push_back(subpassDescription);

	io::graphics::DependencyDescription dependencyDescription{};
	renderPassLayout.dependencyDescriptions_.push_back(dependencyDescription);

	renderPass_ = renderer_->GetGraphicsAPI().CreateRenderPass(renderPassLayout, 1600, 900);
	pipeline_ = renderer_->GetGraphicsAPI().CreatePipeline(pipelineState, *renderPass_);
	renderer_->GetGraphicsAPI().BindRenderPass(renderPass_);
	renderer_->GetGraphicsAPI().BindPipeline(pipeline_);
}

void HelloTriangle::Update()
{
	renderer_->GetGraphicsAPI().Draw();
}
