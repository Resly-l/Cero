#pragma once


class IDeviceHandle
{
public:
	virtual ~IDeviceHandle() = default;

public:
	virtual void* operator*() = 0;
	virtual void* operator&() = 0;
};