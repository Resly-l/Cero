#pragma once
#include "window/application.h"
#include "utility/timer.hpp"


class DepthBufferDemo : public window::Application
{
private:
	std::shared_ptr<graphics::Pipeline> pipeline_;
	std::shared_ptr<graphics::Mesh> cubeMesh_;
	std::shared_ptr<graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<graphics::UniformBuffer> projectionUniformBuffer_;
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