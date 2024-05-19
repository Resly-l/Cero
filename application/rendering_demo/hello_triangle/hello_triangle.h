#pragma once
#include "io/window/application.h"


class HelloTriangle : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::Pipeline> pipeline_;
	std::shared_ptr<io::graphics::RenderTarget> renderTarget_;
	std::shared_ptr<io::graphics::Mesh> mesh_;
	std::shared_ptr<io::graphics::Mesh> mesh2_;

public:
	HelloTriangle();

public:
	virtual void Update() override;
	virtual void Render() override;
};