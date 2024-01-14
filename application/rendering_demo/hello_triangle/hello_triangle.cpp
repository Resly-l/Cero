#include "hello_triangle.h"
#include "io/graphics/renderer.h"
#include "utility/byte_buffer.h"
#include "math/vector.h"

HelloTriangle::HelloTriangle()
{
	Window::SetTitle("Hello Triangle");

	pipelineState_ = std::make_shared<io::graphics::PipelineState>();

	pipelineState_->vertexShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
	pipelineState_->pixelShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";

	pipelineState_->viewport_.width_ = 1600;
	pipelineState_->viewport_.height_ = 900;

	utility::ByteBuffer::Layout vertexLayout;
	vertexLayout.AddAttribute<math::Vec2<float>>("position");
	vertexLayout.AddAttribute<math::Vec3<float>>("color");
	pipelineState_->vertexBuffer.SetLayout(vertexLayout);

	if (auto vertex = pipelineState_->vertexBuffer.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.0f, -0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.7f, 0.75f);
	}

	if (auto vertex = pipelineState_->vertexBuffer.Add())
	{	
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.69f, 0.22f, 0.25f);
	}

	if (auto vertex = pipelineState_->vertexBuffer.Add())
	{
		vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, 0.5f);
		vertex->Get<math::Vec3<float>>("color") = math::Vec3(.0f, .0f, .0f);
	}

	renderer_->SetPipelineState(pipelineState_);
}

void HelloTriangle::Update()
{
}
