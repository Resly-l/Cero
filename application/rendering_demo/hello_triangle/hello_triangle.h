#pragma once
#include "io/window/application.h"


class HelloTriangle : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::RenderTarget> renderTarget_;
	std::shared_ptr<io::graphics::Pipeline> pipeline_;

public:
	HelloTriangle();

public:
	virtual void Update() override;
};