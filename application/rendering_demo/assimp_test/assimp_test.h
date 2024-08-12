#pragma once
#include "window/application.h"

class AssimpTest : public window::Application
{
private:
    std::shared_ptr<graphics::Pipeline> pipeline_;
    std::shared_ptr<graphics::Mesh> mesh_;
	std::shared_ptr<graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<graphics::UniformBuffer> projectionUniformBuffer_;
	utility::ByteBuffer mvBuffer_;
	utility::ByteBuffer pBuffer_;

public:
    AssimpTest();

    virtual void Update() override;
    virtual void Render() override;
};