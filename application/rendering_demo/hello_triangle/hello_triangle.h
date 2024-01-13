#pragma once
#include "io/window/application.h"

namespace io::graphics
{
	struct PipelineState;
}

class HelloTriangle : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::PipelineState> pipelineState_;

public:
	HelloTriangle();

public:
	virtual void Update() override;
};