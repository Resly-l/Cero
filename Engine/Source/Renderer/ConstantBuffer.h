#pragma once


class ConstantBuffer
{
public:
	enum class BindFlag
	{
		bfNONE,
		bfBIND_VS,
		bfBIND_PS
	};

private:
	BindFlag bindFlag = BindFlag::bfNONE;
	void* buffer = nullptr;
	int byteWidth = 0;
	int slot = -1;

public:
	ConstantBuffer(BindFlag bindFlag_in, void* buffer_in, int byteWidth_in);
};