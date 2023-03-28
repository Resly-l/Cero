#pragma once
#include "Window/WindowsMinimal.h"
#include "IDeviceHandle.h"
#include <memory>
#include <array>


class GraphicsDevice
{
private:
	int width;
	int height;

public:
	GraphicsDevice(HWND window, int width, int height);

public:
	std::unique_ptr<IDeviceHandle> CreateSwapChain(HWND window);
	std::unique_ptr<IDeviceHandle> CreateBackBuffer(IDeviceHandle* swapChain);

	void ClearRenderTarget(IDeviceHandle* renderTarget);
	void PresentSwapChain(IDeviceHandle* swapChain, IDeviceHandle* backBuffer);

private:
	void CreateDeviceAndContext(HWND window);
};