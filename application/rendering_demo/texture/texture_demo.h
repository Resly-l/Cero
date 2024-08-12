#pragma once
#include "window/application.h"

class TextureDemo : public window::Application
{
private:
	std::shared_ptr<graphics::Pipeline> pipeline_;
	std::shared_ptr<graphics::Mesh> plane_;
	std::shared_ptr<graphics::Texture> texture_;

public:
	TextureDemo();

private:
	virtual void Update() override;
	virtual void Render() override;
};