#pragma once
#include "Common/Types.h"
#include "IDeviceHandle.h"

class IGraphicsAPI
{
public:
	virtual ~IGraphicsAPI() = default;

public:
	//virtual bool IsVSyncEnabled() const = 0;
	//virtual void SetVSync(bool sync) = 0;

	//virtual std::unique_ptr<IDeviceHandle> CreateSwapChain(int width, int height) = 0;
	//virtual std::unique_ptr<IDeviceHandle> CreateBackBuffer(IDeviceHandle* swapChain, int width, int height) = 0;

	//virtual void ClearRenderTarget(IDeviceHandle* renderTarget) = 0;
	//virtual void PresentSwapChain(IDeviceHandle* swapChain, IDeviceHandle* backBuffer) = 0;
};