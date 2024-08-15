#include "assimp_test.h"
#include "file/model.h"
#include "utility/shader_compiler.h"
#include "math/vector.h"
#include "math/matrix.h"

AssimpTest::AssimpTest()
    : window::Application(graphics::GraphicsAPI::Type::VULKAN)
{
	file::Model brickPlane;
	brickPlane.Load("brick/brick_dissolved.fbx");

	utility::ShaderCompiler::CompileShaders("shader/", "shader/bin/");

	graphics::Texture::Layout textureLayout;
	textureLayout.imagePath_ = brickPlane.diffuseMapPath_;
	textureLayout.stage_ = graphics::ShaderBinding::Stage::PIXEL;
	textureLayout.slot_ = 2;
	brickDiffuse_ = graphicsAPI_->CreateTexture(textureLayout);

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
	modelViewUBLayout.slot_ = 0;
	modelViewUBLayout.stage_ = graphics::ShaderBinding::Stage::VERTEX;
	modelViewUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(modelViewUBLayout);

	graphics::UniformBuffer::Layout projectionUBLayout;
	projectionUBLayout.size_ = (uint32_t)projectionMatrixLayout.GetSizeInBytes();
	projectionUBLayout.slot_ = 1;
	projectionUBLayout.stage_ = graphics::ShaderBinding::Stage::VERTEX;
	projectionUniformBuffer_ = graphicsAPI_->CreateUniformBuffer(projectionUBLayout);

	math::Matrix modelMatrix;
	modelMatrix = math::Matrix::Translation(math::Vector(0.0f, 0.0f, 2.0f, 0.0f));
	mvBuffer_.At(0).Get<math::Matrix>(0) = modelMatrix;
	mvBuffer_.At(0).Get<math::Matrix>(1) = math::Matrix::Identity();
	pBuffer_.At(0).Get<math::Matrix>(0) = math::Matrix::Projection(0.1f, 100.0f, 90.0f, 1.777777f, true);
	modelViewUniformBuffer_->Update(mvBuffer_.GetRawBufferAddress());
	projectionUniformBuffer_->Update(pBuffer_.GetRawBufferAddress());

	graphics::Mesh::Layout meshLayout;
	meshLayout.vertices_ = brickPlane.vertices_;
	meshLayout.indices_ = brickPlane.indices_;
	mesh_ = graphicsAPI_->CreateMesh(meshLayout);

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
	pipelineLayout.vertexInputLayout_ = brickPlane.vertices_.GetLayout().value();
	pipelineLayout.depthFunc_ = graphics::ComparisonFunc::LESS_EQUAL;
	pipelineLayout.descriptor_.outputs.push_back(colorOutput);
	pipelineLayout.descriptor_.outputs.push_back(depthOutput);
	pipelineLayout.descriptor_.resources_.push_back(modelViewUniformBuffer_);
	pipelineLayout.descriptor_.resources_.push_back(projectionUniformBuffer_);
	pipelineLayout.descriptor_.resources_.push_back(brickDiffuse_);
	pipeline_ = graphicsAPI_->CreatePipeline(pipelineLayout);
}

void AssimpTest::Update()
{

}

void AssimpTest::Render()
{
    graphicsAPI_->BindPipeline(pipeline_);
    graphicsAPI_->BindMesh(mesh_);
    graphicsAPI_->BindRenderTarget(graphicsAPI_->GetSwapchainRenderTarget());
	graphicsAPI_->Draw();
}