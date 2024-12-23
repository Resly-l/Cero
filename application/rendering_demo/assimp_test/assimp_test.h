#pragma once
#include "window/application.h"
#include "utility/timer.hpp"

class AssimpTest : public window::Application
{
private:
    std::shared_ptr<graphics::Pipeline> pipeline_;
    std::shared_ptr<graphics::Mesh> mesh_;
	std::shared_ptr<graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<graphics::UniformBuffer> projectionUniformBuffer_;

	std::shared_ptr<graphics::Texture> defaultTexture_;
	std::shared_ptr<graphics::Material> defaultMaterial_;
	std::shared_ptr<graphics::RenderPass> renderPass_;

	utility::ByteBuffer mvBuffer_;
	utility::ByteBuffer pBuffer_;

	utility::Timer<float> timer_;
	float yaw_ = 0.0f;

public:
    AssimpTest();

    virtual void Tick() override;
};