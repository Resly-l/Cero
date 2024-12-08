#include "assimp_test.h"
#include "file/model.h"
#include "graphics/mesh.h"
#include "utility/shader_compiler.h"
#include "utility/timer.hpp"
#include "math/vector.h"
#include "math/matrix.h"
#include <iostream>

AssimpTest::AssimpTest()
    : window::Application(graphics::GraphicsAPI::Type::VULKAN)
{
	file::Model brickPlane;
	brickPlane.Load("bunny.obj");

	utility::ShaderCompiler::CompileShaders("shader/", "shader/bin/");

	graphics::Mesh::Layout meshLayout;
	meshLayout.vertices_ = brickPlane.meshes_[0].vertices_;
	meshLayout.indices_ = brickPlane.meshes_[0].indices_;
	mesh_ = graphicsAPI_->CreateMesh(meshLayout);

	utility::ByteBuffer::Layout modelViewMatrixLayout;
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	modelViewMatrixLayout.AddAttribute<math::Matrix>();
	mvBuffer_.SetLayout(modelViewMatrixLayout);
	mvBuffer_.Add();

	utility::ByteBuffer::Layout projectionMatrixLayout;
	projectionMatrixLayout.AddAttribute<math::Matrix>();
	pBuffer_.SetLayout(projectionMatrixLayout);
	pBuffer_.Add();

	graphics::UniformBuffer::Layout modelViewUBLayout;
	modelViewUBLayout.size_ = (uint32_t)modelViewMatrixLayout.GetSizeInBytes();
	modelViewUBLayout.persistentMapping_ = true;
	modelViewUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(modelViewUBLayout);

	graphics::UniformBuffer::Layout projectionUBLayout;
	projectionUBLayout.size_ = (uint32_t)projectionMatrixLayout.GetSizeInBytes();
	projectionUBLayout.persistentMapping_ = true;
	projectionUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(projectionUBLayout);

	math::Matrix modelMatrix;
	modelMatrix = math::Matrix::Scale(5.0f);
	modelMatrix *= math::Matrix::Translation(math::Vector(0.0f, 0.0f, 2.0f, 0.0f));
	mvBuffer_.At(0).Get<math::Matrix>(0) = modelMatrix;
	mvBuffer_.At(0).Get<math::Matrix>(1) = math::Matrix::Identity();
	pBuffer_.At(0).Get<math::Matrix>(0) = math::Matrix::Projection(0.1f, 100.0f, 90.0f, 1.777777f, true);
	modelViewUniformBuffer_->Update(mvBuffer_.GetRawBufferAddress());
	projectionUniformBuffer_->Update(pBuffer_.GetRawBufferAddress());

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
	pipelineLayout.vertexShaderPath_ = L"shader/bin/model.vert.spv";
	pipelineLayout.pixelShaderPath_ = L"shader/bin/model.frag.spv";
	pipelineLayout.vertexInputLayout_ = brickPlane.meshes_[0].vertices_.GetLayout().value();
	pipelineLayout.depthFunc_ = graphics::ComparisonFunc::LESS_EQUAL;
	pipelineLayout.shaderDescriptor_.outputs.push_back(colorOutput);
	pipelineLayout.shaderDescriptor_.outputs.push_back(depthOutput);

	graphics::ShaderDescriptor::Binding binding;
	binding.stage_ = graphics::ShaderStage::VERTEX;
	binding.type_ = graphics::ShaderBinding::Type::UNIFORM_BUFFER;
	binding.slot_ = 0;
	pipelineLayout.shaderDescriptor_.bindings_.push_back(binding);

	binding.slot_ = 1;
	pipelineLayout.shaderDescriptor_.bindings_.push_back(binding);

	binding.stage_ = graphics::ShaderStage::PIXEL;
	binding.type_ = graphics::ShaderBinding::Type::TEXTURE_2D;
	binding.slot_ = 2;
	pipelineLayout.shaderDescriptor_.bindings_.push_back(binding);

	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);

	pipeline_->BindShaderBinding(modelViewUniformBuffer_, 0);
	pipeline_->BindShaderBinding(projectionUniformBuffer_, 1);
}

void AssimpTest::Update()
{
	float deltaSeconds = timer_.Mark();

	yaw_ += deltaSeconds * 2.0f;
	math::Matrix modelMatrix;
	modelMatrix = math::Matrix::Scale(5.0f);
	modelMatrix *= math::Matrix::RotationY(yaw_);
	modelMatrix *= math::Matrix::Translation(math::Vector(0.0f, -0.5f, 2.0f, 0.0f));
	mvBuffer_.At(0).Get<math::Matrix>(0) = modelMatrix;

	modelViewUniformBuffer_->Update(mvBuffer_.GetRawBufferAddress());

	static float elapsed = 0.0f;
	static int fpsCount = 0;

	if ((elapsed += deltaSeconds) > 1.0f)
	{
		elapsed -= 1.0f;
		std::cout << "fps : " << fpsCount << std::endl;
		fpsCount = 0;
	}
	fpsCount++;
}

void AssimpTest::Render()
{
    graphicsAPI_->BindPipeline(pipeline_);
    graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
    graphicsAPI_->BindMesh(mesh_);
	graphicsAPI_->Draw();
}