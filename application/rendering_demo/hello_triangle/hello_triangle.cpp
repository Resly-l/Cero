#include "hello_triangle.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"
#include "core/math/matrix.h"
#include "io/graphics/uniform_buffer.h"
#include <iostream>

#include "io/file/shader_compiler.h"

using namespace io::graphics;

HelloTriangle::HelloTriangle()
	: Application(GraphicsAPI::VULKAN)
{
	Window::SetTitle("Hello Triangle");
	Window::SetResizability(false);

	io::file::ShaderCompiler::CompileShaders("shader/", "shader/bin/");

	utility::ByteBuffer::Layout modelViewMatrixLayout;
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	mvBuffer_.SetLayout(modelViewMatrixLayout);
	mvBuffer_.Add();

	utility::ByteBuffer::Layout projectionMatrixLayout;
	projectionMatrixLayout.AddAttribute<math::Matrix>();
	pBuffer_.SetLayout(projectionMatrixLayout);
	pBuffer_.Add();

	UniformBuffer::Layout modelViewUBLayout;
	modelViewUBLayout.size_ = (uint32_t)modelViewMatrixLayout.GetSizeInBytes();
	modelViewUBLayout.slot_ = 0;
	modelViewUBLayout.stage_ = ShaderBinding::Stage::VERTEX;
	modelViewUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(modelViewUBLayout);

	UniformBuffer::Layout projectionUBLayout;
	projectionUBLayout.size_ = (uint32_t)projectionMatrixLayout.GetSizeInBytes();
	projectionUBLayout.slot_ = 1;
	projectionUBLayout.stage_ = ShaderBinding::Stage::VERTEX;
	projectionUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(projectionUBLayout);

	Pipeline::Layout pipelineLayout;
	pipelineLayout.primitiveTopology_ = PrimitiveTopology::TRIANGLE_LIST;
	pipelineLayout.vertexShaderPath_ = L"shader/bin/hello_triangle.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"shader/bin/hello_triangle.frag.spv";
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Float2>();
	pipelineLayout.vertexInputLayout_.AddAttribute<math::Float3>();
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
	pipelineLayout.descriptor_.resources_.push_back(modelViewUniformBuffer_);
	pipelineLayout.descriptor_.resources_.push_back(projectionUniformBuffer_);

	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);
	renderTarget_ = pipeline_->CreateRenderTarget(1600, 900);

	Mesh::Layout meshLayout;
	meshLayout.vertices_.SetLayout(pipelineLayout.vertexInputLayout_);

	auto vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(-0.35f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.98f, 0.75f, 0.85f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(0.35f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.69f, 0.22f, 0.25f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(0.35f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.98f, 0.75f, 0.85f);
	vertex = meshLayout.vertices_.Add();
	vertex.Get<math::Float2>(0) = math::Float2(-0.35f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.15f, 0.15f, 0.15f);

	meshLayout.indices_ = { 0, 1, 2, 2, 3, 0 };
	mesh_ = graphicsAPI_->CreateMesh(meshLayout);
	meshLayout.indices_ = { 2,3,0 };
	mesh2_ = graphicsAPI_->CreateMesh(meshLayout);
}

void HelloTriangle::Update()
{
	auto deltaSeconds = timer_.Mark();
	yaw_ += (float)deltaSeconds;

	math::Matrix modelMatrix = math::Matrix::RotationY(yaw_);
	modelMatrix *= math::Matrix::Translation(math::Vector(0.0f, 0.0f, 1.5f + sinf(yaw_), 0.0f));
	
	mvBuffer_.At(0).Get<math::Matrix>(0) = modelMatrix;
	mvBuffer_.At(0).Get<math::Matrix>(1) = math::Matrix::Identity();
	pBuffer_.At(0).Get<math::Matrix>(0) = math::Matrix::Projection(0.1f, 100.0f, 90.0f, 1.777777f);

	modelViewUniformBuffer_->Update(mvBuffer_.GetRawBufferAddress());
	projectionUniformBuffer_->Update(pBuffer_.GetRawBufferAddress());
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