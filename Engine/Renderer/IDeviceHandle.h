#pragma once


class IDeviceHandle
{
public:
	virtual ~IDeviceHandle() = default;

public:
	virtual void* Get() = 0;
	virtual void* GetAddressOf() = 0;
};