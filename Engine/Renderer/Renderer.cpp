#include "Renderer.h"

Renderer::Renderer(HWND window, int width, int height)
	:
	device(window, width, height)
{
	swapChain = device.CreateSwapChain(window);
	backBuffer = device.CreateBackBuffer(swapChain.get());
}

void Renderer::BeginFrame()
{
	device.ClearRenderTarget(backBuffer.get());
}

void Renderer::EndFrame()
{
	device.PresentSwapChain(swapChain.get(), backBuffer.get());
}
