#include "GraphicsDevice.h"
#include <vector>

#ifndef USE_SOFTWARE_RENDERING
#include <d3d11.h>
#include <wrl.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#endif

#ifdef USE_SOFTWARE_RENDERING
HDC mainDC;
HDC bufferDC;
#else
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<ID3D11Device> device;
ComPtr<ID3D11DeviceContext> deviceContext;
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
		ComPtr<IDXGISwapChain> swapChain;

	public:
		virtual void* Get() override { return swapChain.Get(); }
		virtual void* GetAddressOf() override { return swapChain.GetAddressOf(); }
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
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ComPtr<IDXGIDevice> dxgiDevice;
	ComPtr<IDXGIAdapter> dxgiAdapter;
	ComPtr<IDXGIFactory> dxgiFactory;
	DXGI_ADAPTER_DESC desc = {};

	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	dxgiAdapter->GetDesc(&desc);
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	if (FAILED(dxgiFactory->CreateSwapChain(device.Get(), &sd, (IDXGISwapChain**)swapChain->GetAddressOf())))
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
		std::vector<int> colorBuffer;

	public:
		SoftwareRenderingBackBuffer(int width, int height) : colorBuffer(width * height) {}

	public:
		virtual void* Get() override { return &colorBuffer; }
		virtual void* GetAddressOf() override { return nullptr; }
	};

	return std::make_unique<SoftwareRenderingBackBuffer>(width, height);
#else
	class D3D11BackBuffer : public IDeviceHandle
	{
	private:
		ComPtr<ID3D11RenderTargetView> renderTargetView;

	public:
		virtual void* Get() override { return renderTargetView.Get(); }
		virtual void* GetAddressOf() override { return renderTargetView.GetAddressOf(); }
	};

	auto backBuffer = std::make_unique<D3D11BackBuffer>();

	ComPtr<ID3D11Resource> resource;
	((IDXGISwapChain*)swapChain->Get())->GetBuffer(0, __uuidof(ID3D11Resource), &resource);
	if (FAILED(device->CreateRenderTargetView(resource.Get(), nullptr, (ID3D11RenderTargetView**)backBuffer->GetAddressOf())))
	{
		MessageBox(nullptr, L"Failed to create back buffer", L"Error", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	}

	deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)backBuffer->GetAddressOf(), nullptr);

	return backBuffer;
#endif
}

void GraphicsDevice::ClearRenderTarget(IDeviceHandle* renderTarget)
{
#ifdef USE_SOFTWARE_RENDERING
	auto colorBuffer = (std::vector<int>*)renderTarget->Get();
	memset(colorBuffer->data(), 0, colorBuffer->size() * sizeof(int));
#else
	static float clearColor[4] = { 0.03f, 0.08f, 0.08f };
	deviceContext->ClearRenderTargetView((ID3D11RenderTargetView*)renderTarget->Get(), clearColor);
#endif
}

void GraphicsDevice::PresentSwapChain(IDeviceHandle* swapChain, IDeviceHandle* backBuffer)
{
#ifdef USE_SOFTWARE_RENDERING
	auto bitmap = CreateBitmap(width, height, 1, sizeof(unsigned int) * 8, ((std::vector<int>*)backBuffer->Get())->data());

	SelectObject(bufferDC, bitmap);
	BitBlt(mainDC, 0, 0, width, height, bufferDC, 0, 0, SRCCOPY);

	DeleteObject(bitmap);
#else
	((IDXGISwapChain*)swapChain->Get())->Present(0, 0);
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
