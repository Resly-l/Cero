#include "hello_triangle.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"
#include "core/math/matrix.h"
#include <iostream>

using namespace io::graphics;

HelloTriangle::HelloTriangle()
	: Application(GraphicsAPI::VULKAN)
{
	Window::SetTitle("Hello Triangle");
	Window::SetResizability(false);

	utility::ByteBuffer::Layout mvLayout;
	mvLayout.AddAttribute<math::Matrix<float>>();
	mvLayout.AddAttribute<math::Matrix<float>>();
	mvBuffer_.SetLayout(mvLayout);
	mvBuffer_.Add();

	utility::ByteBuffer::Layout pLayout;
	pLayout.AddAttribute<math::Matrix<float>>();
	pBuffer_.SetLayout(pLayout);
	pBuffer_.Add();

	Pipeline::Layout pipelineLayout;
	pipelineLayout.vertexShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vector2d<float>>();
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Vector3d<float>>();
	pipelineLayout.depthFunc_ = ComparisonFunc::LESS_EQUAL;
	ShaderDescriptor::Output output{};
	output.width_ = 1600;
	output.height_ = 900;
	output.format_ = ImageFormat::B8G8R8A8_UNORM;
	output.usage_ = ImageUsage::COLOR_ATTACHMENT;
	output.loadOp_ = ImageOperation::CLEAR;
	output.storeOp_ = ImageOperation::STORE;
	pipelineLayout.descriptor_.outputs.push_back(output);
	output.format_ = ImageFormat::D32_SFLOAT_U8_UINT;
	output.usage_ = ImageUsage::DEPTH_STENCIL;
	pipelineLayout.descriptor_.outputs.push_back(output);

	ShaderDescriptor::Binding binding{};
	binding.stage_ = ShaderDescriptor::Binding::Stage::VERTEX;
	binding.type_ = ShaderDescriptor::Binding::Type::UNIFORM;
	binding.size_ = (uint32_t)mvBuffer_.GetSizeInBytes();
	pipelineLayout.descriptor_.bindings_.push_back(binding);
	binding.size_ = (uint32_t)pBuffer_.GetSizeInBytes();
	pipelineLayout.descriptor_.bindings_.push_back(binding);

	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);
	renderTarget_ = pipeline_->CreateRenderTarget(1600, 900);

	Mesh::Layout meshLayout;
	meshLayout.vertices_.SetLayout(pipelineLayout.vertexInputLayout_);

	auto vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d(-0.35f, -0.5f);
	vertex.Get<math::Vector3d<float>>(1) = math::Vector3d(0.98f, 0.75f, 0.85f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d(0.35f, -0.5f);
	vertex.Get<math::Vector3d<float>>(1) = math::Vector3d(0.69f, 0.22f, 0.25f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d(0.35f, 0.5f);
	vertex.Get<math::Vector3d<float>>(1) = math::Vector3d(0.98f, 0.75f, 0.85f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Vector2d<float>>(0) = math::Vector2d(-0.35f, 0.5f);
	vertex.Get<math::Vector3d<float>>(1) = math::Vector3d(0.15f, 0.15f, 0.15f);

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
	auto deltaSeconds = timer.Mark();
	yaw_ += (float)deltaSeconds;

	math::Matrix<float> modelMatrix = math::Matrix<float>::RotationY(yaw_);
	modelMatrix *= math::Matrix<float>::Translation(math::Vector<float>(0.0f, 0.0f, 1.5f + sinf(yaw_), 0.0f));
	
	mvBuffer_.At(0).Get<math::Matrix<float>>(0) = modelMatrix;
	mvBuffer_.At(0).Get<math::Matrix<float>>(1) = math::Matrix<float>::Identity();
	pBuffer_.At(0).Get<math::Matrix<float>>(0) = math::Matrix<float>::Projection(0.1f, 100.0f, 90.0f, 1.777777f);

	pipeline_->UpdateUniformBuffer(0, mvBuffer_);
	pipeline_->UpdateUniformBuffer(1, pBuffer_);
}

void HelloTriangle::Render()
{
	graphicsAPI_->BindPipeline(pipeline_);
	/*graphicsAPI_->BindRenderTarget(renderTarget_);
	graphicsAPI_->BindMesh(mesh2_);
	graphicsAPI_->Draw();*/

	graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->BindMesh(mesh_);
	graphicsAPI_->Draw();
}