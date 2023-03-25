#include "GraphicsDevice.h"
#include <vector>

#ifndef USE_SOFTWARE_RENDERING
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#endif

GraphicsDevice::GraphicsDevice(HWND window, int width_in, int height_in)
	:
	width(width_in),
	height(height_in)
{
	CreateDeviceAndContext(window);
}

std::unique_ptr<IDeviceHandle> GraphicsDevice::CreateSwapChain(HWND window)
{
#ifdef USE_SOFTWARE_RENDERING
	return nullptr;
#else
	class D3D11SwapChain : public IDeviceHandle
	{
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	public:
		virtual void* operator*() override { return swapChain.Get(); }
		virtual void* operator&() override { return swapChain.GetAddressOf(); }
	};

	auto swapChain = std::make_unique<D3D11SwapChain>();

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = window;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	DXGI_ADAPTER_DESC desc = {};
	dxgiAdapter->GetDesc(&desc);

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(dxgiFactory->CreateSwapChain(device.Get(), &sd, (IDXGISwapChain**)swapChain->operator&())))
	{
		MessageBox(nullptr, L"Failed to create swap chain", L"Error", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	};

	return swapChain;
#endif
}

std::unique_ptr<IDeviceHandle> GraphicsDevice::CreateBackBuffer(IDeviceHandle* swapChain)
{
#ifdef USE_SOFTWARE_RENDERING
	class SoftwareRenderingBackBuffer : public IDeviceHandle
	{
	private:
		std::unique_ptr<std::vector<int>> colorBuffer;

	public:
		SoftwareRenderingBackBuffer(int width, int height) { colorBuffer = std::make_unique<std::vector<int>>(width * height); }

	public:
		virtual void* operator*() override { return colorBuffer.get(); }
		virtual void* operator&() override { return nullptr; }
	};

	return std::make_unique<SoftwareRenderingBackBuffer>(width, height);
#else
	class D3D11BackBuffer : public IDeviceHandle
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	public:
		virtual void* operator*() override { return renderTargetView.Get(); }
		virtual void* operator&() override { return renderTargetView.GetAddressOf(); }
	};

	auto backBuffer = std::make_unique<D3D11BackBuffer>();

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	((IDXGISwapChain*)swapChain->operator*())->GetBuffer(0, __uuidof(ID3D11Resource), &resource);
	if (FAILED(device->CreateRenderTargetView(resource.Get(), nullptr, (ID3D11RenderTargetView**)backBuffer->operator&())))
	{
		MessageBox(nullptr, L"Failed to create back buffer", L"Error", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	}

	deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)backBuffer->operator&(), nullptr);

	return backBuffer;
#endif
}

void GraphicsDevice::ClearBackBuffer(IDeviceHandle* backBuffer)
{
#ifdef USE_SOFTWARE_RENDERING
	auto colorBuffer = (std::vector<int>*)backBuffer->operator*();
	memset(colorBuffer->data(), 0, colorBuffer->size() * sizeof(int));
#else
	static float clearColor[4] = {};
	deviceContext->ClearRenderTargetView((ID3D11RenderTargetView*)backBuffer->operator*(), clearColor);
#endif
}

void GraphicsDevice::PresentSwapChain(IDeviceHandle* swapChain, IDeviceHandle* backBuffer)
{
#ifdef USE_SOFTWARE_RENDERING
	auto bitmap = CreateBitmap(width, height, 1, sizeof(unsigned int) * 8, ((std::vector<int>*)backBuffer->operator*())->data());

	SelectObject(bufferDC, bitmap);
	BitBlt(mainDC, 0, 0, width, height, bufferDC, 0, 0, SRCCOPY);

	DeleteObject(bitmap);
#else
	((IDXGISwapChain*)swapChain->operator*())->Present(0, 0);
#endif
}

void GraphicsDevice::CreateDeviceAndContext(HWND window)
{
#ifdef USE_SOFTWARE_RENDERING
	mainDC = GetDC(window);
	bufferDC = CreateCompatibleDC(mainDC);
#else
	uint32_t flags = 0;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &device, {}, &deviceContext)))
	{
		MessageBox(nullptr, L"Failed to create graphics device", L"Error", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	}
#endif
}
