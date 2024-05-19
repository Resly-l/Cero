#include "hello_triangle.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"

using namespace io::graphics;

HelloTriangle::HelloTriangle()
	: Application(GraphicsAPIType::Vulkan)
{
	Window::SetTitle("Hello Triangle");
	Window::SetResizability(false);

	Pipeline::Layout pipelineLayout;
	pipelineLayout.vertexShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vec2<float>>("position");
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vec3<float>>("color");
	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);

	RenderTarget::Layout renderTargetLayout;
	renderTargetLayout.width_ = 1600;
	renderTargetLayout.height_ = 900;
	RenderTarget::AttachmentDescription attachmentDesc;
	attachmentDesc.format_ = ImageFormat::B8G8R8A8_UNORM;
	attachmentDesc.width_ = 1600;
	attachmentDesc.height_ = 900;
	attachmentDesc.usage_ = ImageUsage::COLOR_ATTACHMENT;
	renderTargetLayout.attachments_.push_back(attachmentDesc);
	attachmentDesc.format_ = ImageFormat::D32_SFLOAT_U8_UINT;
	attachmentDesc.usage_ = ImageUsage::DEPTH_STENCIL;
	renderTargetLayout.attachments_.push_back(attachmentDesc);
	renderTarget_ = graphicsAPI_->CreateRenderTarget(renderTargetLayout);

	Mesh::Layout meshLayout;
	utility::ByteBuffer::Layout vertexBufferLayout;
	vertexBufferLayout.AddAttribute<math::Vec2<float>>("position");
	vertexBufferLayout.AddAttribute<math::Vec3<float>>("color");
	meshLayout.vertices_.SetLayout(vertexBufferLayout);
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, -0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, -0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.69f, 0.22f, 0.25f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.15f, 0.15f, 0.15f);
	}
	meshLayout.indices_ = { 0,1,2 };
	mesh_ = graphicsAPI_->CreateMesh(meshLayout);
	meshLayout.indices_ = { 2,3,0 };
	mesh2_ = graphicsAPI_->CreateMesh(meshLayout);
}

void HelloTriangle::Update()
{

}

void HelloTriangle::Render()
{
	graphicsAPI_->BindPipeline(pipeline_);
	graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->BindMesh(mesh_);
	graphicsAPI_->Draw();
	//graphicsAPI_->BindRenderTarget(renderTarget_);
	graphicsAPI_->BindMesh(mesh2_);
	graphicsAPI_->Draw();
}