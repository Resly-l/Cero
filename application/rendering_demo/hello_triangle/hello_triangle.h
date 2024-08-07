#pragma once
#include "io/window/application.h"
#include "utility/timer.hpp"


class HelloTriangle : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::Pipeline> pipeline_;
	std::shared_ptr<io::graphics::RenderTarget> renderTarget_;
	std::shared_ptr<io::graphics::Mesh> mesh_;
	std::shared_ptr<io::graphics::Mesh> mesh2_;
	std::shared_ptr<io::graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<io::graphics::UniformBuffer> projectionUniformBuffer_;

	utility::ByteBuffer mvBuffer_;
	utility::ByteBuffer pBuffer_;
	float yaw_ = 0.0f;

	utility::Timer<double> timer_;

public:
	HelloTriangle();

private:
	virtual void Update() override;
	virtual void Render() override;
};