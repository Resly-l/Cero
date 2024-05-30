#include "hello_triangle.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"
#include "core/math/matrix.h"
#include <iostream>

using namespace io::graphics;

HelloTriangle::HelloTriangle()
	: Application(GraphicsAPIType::Vulkan)
{
	Window::SetTitle("Hello Triangle");
	Window::SetResizability(false);

	Pipeline::Layout pipelineLayout;
	pipelineLayout.vertexShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vector2d<float>>("position");
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vector3d<float>>("color");
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
	vertexBufferLayout.AddAttribute<math::Vector2d<float>>("position");
	vertexBufferLayout.AddAttribute<math::Vector3d<float>>("color");
	meshLayout.vertices_.SetLayout(vertexBufferLayout);
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vector2d<float>>("position") = math::Vector2d(-0.35f, -0.5f);
		vertex->Get<math::Vector3d<float>>("color") = math::Vector3d(0.98f, 0.75f, 0.85f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vector2d<float>>("position") = math::Vector2d(0.35f, -0.5f);
		vertex->Get<math::Vector3d<float>>("color") = math::Vector3d(0.69f, 0.22f, 0.25f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vector2d<float>>("position") = math::Vector2d(0.35f, 0.5f);
		vertex->Get<math::Vector3d<float>>("color") = math::Vector3d(0.98f, 0.75f, 0.85f);
	}
	if (auto vertex = meshLayout.vertices_.Add())
	{
		vertex->Get<math::Vector2d<float>>("position") = math::Vector2d(-0.35f, 0.5f);
		vertex->Get<math::Vector3d<float>>("color") = math::Vector3d(0.15f, 0.15f, 0.15f);
	}
	meshLayout.indices_ = { 0,1,2 };
	mesh_ = graphicsAPI_->CreateMesh(meshLayout);
	meshLayout.indices_ = { 2,3,0 };
	mesh2_ = graphicsAPI_->CreateMesh(meshLayout);
}

HelloTriangle::~HelloTriangle()
{
	graphicsAPI_->WaitIdle();
}

void HelloTriangle::Update()
{
	math::Matrix2x2<float> mat2d;
	mat2d = math::Matrix2x2<float>::Identity();

	math::Matrix3x3<float> mat3d;
	mat3d.v_[0] = math::Vector3d<float>(-1.02f, -2.0f, 34.0f);
	mat3d.v_[1] = math::Vector3d<float>(3.0f, 5.0f, 8.0f);
	mat3d.v_[2] = math::Vector3d<float>(1.02f, 2.0f, -34.0f);
	mat3d = mat2d;

	math::Matrix<float> mat;
	mat = math::Matrix<float>::RotationX(3.141592f * 0.5f);
	mat = math::Matrix<float>::Projection(0.5f, 1000.0f, 3.141592f * 0.66f, 16.0f / 9.0f);

	timer.Peek();
	//std::cout << timer.Mark() << std::endl;
}

void HelloTriangle::Render()
{
	graphicsAPI_->BindPipeline(pipeline_);

	graphicsAPI_->BindRenderTarget(renderTarget_);
	graphicsAPI_->BindMesh(mesh2_);
	graphicsAPI_->Draw();

	graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->BindMesh(mesh_);
	graphicsAPI_->Draw();
}