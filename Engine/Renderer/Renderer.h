#pragma once
#include "GraphicsDevice.h"


class Renderer
{
private:
	GraphicsDevice device;
	std::unique_ptr<IDeviceHandle> swapChain;
	std::unique_ptr<IDeviceHandle> backBuffer;

public:
	Renderer(HWND window, int width, int height);

public:
	void BeginFrame();
	void EndFrame();
};