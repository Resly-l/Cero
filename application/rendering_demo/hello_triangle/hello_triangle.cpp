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

	pipeline_ = renderer_->CreatePipeline(pipelineState);
}

void HelloTriangle::Update()
{
	renderer_->Render(pipeline_);
}
