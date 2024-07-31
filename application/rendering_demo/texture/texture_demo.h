#pragma once
#include "io/window/application.h"

class TextureDemo : public io::window::Application
{
public:
	TextureDemo();

private:
	virtual void Update() override;
	virtual void Render() override;
};