#pragma once
#include "io/window/application.h"

class AssimpTest : public io::window::Application
{
private:
    std::shared_ptr<io::graphics::Pipeline> pipeline_;
    std::shared_ptr<io::graphics::Mesh> mesh_;
	std::shared_ptr<io::graphics::UniformBuffer> modelViewUniformBuffer_;
	std::shared_ptr<io::graphics::UniformBuffer> projectionUniformBuffer_;
	utility::ByteBuffer mvBuffer_;
	utility::ByteBuffer pBuffer_;

public:
    AssimpTest();

    virtual void Update() override;
    virtual void Render() override;
};