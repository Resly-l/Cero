#pragma once
#include "window/application.h"
#include "utility/timer.hpp"


class HelloTriangle : public window::Application
{
private:
	std::shared_ptr<graphics::Pipeline> pipeline_;
	std::shared_ptr<graphics::RenderTarget> renderTarget_;
	std::shared_ptr<graphics::Mesh> mesh_;
	std::shared_ptr<graphics::Mesh> mesh2_;
	std::shared_ptr<graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<graphics::UniformBuffer> projectionUniformBuffer_;

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