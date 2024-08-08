#pragma once
#include "io/window/application.h"
#include "utility/timer.hpp"


class DepthBufferDemo : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::Pipeline> pipeline_;
	std::shared_ptr<io::graphics::Mesh> cubeMesh_;
	std::shared_ptr<io::graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<io::graphics::UniformBuffer> projectionUniformBuffer_;
	utility::ByteBuffer mvBuffer_;
	utility::ByteBuffer pBuffer_;

	utility::Timer<double> timer_;
	float yaw_ = 0.0f;

public:
	DepthBufferDemo();

public:
	virtual void Update() override;
	virtual void Render() override;
};