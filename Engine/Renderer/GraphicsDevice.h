#pragma once
#include "Window/WindowsMinimal.h"
#include "IDeviceHandle.h"
#include <memory>
#include <array>

#ifndef USE_SOFTWARE_RENDERING
#include <d3d11.h>
#include <wrl.h>
#endif

class GraphicsDevice
{
private:
#ifdef USE_SOFTWARE_RENDERING
	HDC mainDC;
	HDC bufferDC;
#else
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
#endif

	int width;
	int height;

public:
	GraphicsDevice(HWND window, int width, int height);

public:
	std::unique_ptr<IDeviceHandle> CreateSwapChain(HWND window);
	std::unique_ptr<IDeviceHandle> CreateBackBuffer(IDeviceHandle* swapChain);

	void ClearBackBuffer(IDeviceHandle* backBuffer);
	void PresentSwapChain(IDeviceHandle* swapChain, IDeviceHandle* backBuffer);

private:
	void CreateDeviceAndContext(HWND window);
};