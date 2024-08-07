#pragma once
#include "io/window/application.h"

class TextureDemo : public io::window::Application
{
private:
	std::shared_ptr<io::graphics::Pipeline> pipeline_;
	std::shared_ptr<io::graphics::Mesh> plane_;
	std::shared_ptr<io::graphics::Texture> texture_;

public:
	TextureDemo();

private:
	virtual void Update() override;
	virtual void Render() override;
};