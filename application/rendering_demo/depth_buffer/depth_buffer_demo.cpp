#include "depth_buffer_demo.h"
#include "core/math/vector.h"
#include "core/math/matrix.h"
#include "io/file/shader_compiler.h"
#include "io/file/image.h"

DepthBufferDemo::DepthBufferDemo()
	: io::window::Application(io::graphics::GraphicsAPI::VULKAN)
{
	io::file::ShaderCompiler::CompileShaders("shader/", "shader/bin/");
	
	io::graphics::Mesh::Layout cubeLayout;
	utility::ByteBuffer::Layout vertexLayout;
	vertexLayout.AddAttribute<math::Float3>();
	vertexLayout.AddAttribute<math::Float3>();
	cubeLayout.vertices_.SetLayout(vertexLayout);
	auto vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(-0.5f, 0.5f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.5f, 0.0f, 0.5f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(0.5f, 0.5f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(1.0f, 0.0f, 0.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(0.5f, -0.5f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.0f, 0.0f, 1.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(-0.5f, -0.5f, -0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.5f, 1.0f, 0.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(-0.5f, 0.5f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(1.0f, 1.0f, 1.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(0.5f, 0.5f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(1.0f, 0.5f, 0.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(0.5f, -0.5f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(0.5f, 1.0f, 1.0f);
	vertex = cubeLayout.vertices_.Add();
	vertex.Get<math::Float3>(0) = math::Float3(-0.5f, -0.5f, 0.5f);
	vertex.Get<math::Float3>(1) = math::Float3(1.0f, 0.4f, 0.4f);

	cubeLayout.indices_ = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		4, 5, 1, 1, 0, 4,
		6, 7, 3, 3, 2, 6,
		4, 0, 3, 3, 7, 4,
		1, 5, 6, 6, 2, 1
	};

	cubeMesh_ = graphicsAPI_->CreateMesh(cubeLayout);

	utility::ByteBuffer::Layout modelViewMatrixLayout;
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	mvBuffer_.SetLayout(modelViewMatrixLayout);
	mvBuffer_.Add();

	utility::ByteBuffer::Layout projectionMatrixLayout;
	projectionMatrixLayout.AddAttribute<math::Matrix>();
	pBuffer_.SetLayout(projectionMatrixLayout);
	pBuffer_.Add();

	io::graphics::UniformBuffer::Layout modelViewUBLayout;
	modelViewUBLayout.size_ = (uint32_t)modelViewMatrixLayout.GetSizeInBytes();
	modelViewUBLayout.slot_ = 0;
	modelViewUBLayout.stage_ = io::graphics::ShaderBinding::Stage::VERTEX;
	modelViewUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(modelViewUBLayout);

	io::graphics::UniformBuffer::Layout projectionUBLayout;
	projectionUBLayout.size_ = (uint32_t)projectionMatrixLayout.GetSizeInBytes();
	projectionUBLayout.slot_ = 1;
	projectionUBLayout.stage_ = io::graphics::ShaderBinding::Stage::VERTEX;
	projectionUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(projectionUBLayout);

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
	pipelineLayout.vertexShaderPath_ = L"shader/bin/depth_buffer.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"shader/bin/depth_buffer.frag.spv";
	pipelineLayout.vertexInputLayout_ = vertexLayout;
	pipelineLayout.depthFunc_ = io::graphics::ComparisonFunc::LESS_EQUAL;
	pipelineLayout.descriptor_.outputs.push_back(colorOutput);
	pipelineLayout.descriptor_.outputs.push_back(depthOutput);
	pipelineLayout.descriptor_.resources_.push_back(modelViewUniformBuffer_);
	pipelineLayout.descriptor_.resources_.push_back(projectionUniformBuffer_);
	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);
}

void DepthBufferDemo::Update()
{
	double deltaSeconds = timer_.Mark();
	yaw_ += (float)deltaSeconds;

	math::Matrix modelMatrix = math::Matrix::RotationY(yaw_) * math::Matrix::RotationX(yaw_);
	modelMatrix *= math::Matrix::Translation(math::Vector(0.0f, 0.0f, 2.0f + sinf(yaw_) * 0.5f, 0.0f));

	mvBuffer_.At(0).Get<math::Matrix>(0) = modelMatrix;
	mvBuffer_.At(0).Get<math::Matrix>(1) = math::Matrix::Identity();
	pBuffer_.At(0).Get<math::Matrix>(0) = math::Matrix::Projection(0.1f, 100.0f, 90.0f, 1.777777f);

	modelViewUniformBuffer_->Update(mvBuffer_.GetRawBufferAddress());
	projectionUniformBuffer_->Update(pBuffer_.GetRawBufferAddress());
}

void DepthBufferDemo::Render()
{
	graphicsAPI_->BindPipeline(pipeline_);
	graphicsAPI_->BindMesh(cubeMesh_);
	graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->Draw();
}