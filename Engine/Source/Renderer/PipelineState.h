#pragma once
#include "Common/Types.h"
#include "IDeviceHandle.h"
#include "ConstantBuffer.h"

struct PipelineState
{
	enum class PrimitiveTopology
	{
		ptNONE,
		ptLINE_LIST,
		ptTRIANGLE_LIST
	};
	enum class ComparisonFunc
	{
		cfNONE,
		cfLESS,
		cf_LESS_EQUAL,
		cf_EQUAL,
		cf_NOT_EQUAL,
		cf_GREATER,
		cf_GREATER_EQUAL,
		cf_ALWAYS
	};
	enum class RasterizerState
	{
		rsNONE,
		rsCULL_NONE,
		rsCULL_FRONT,
		rsCULL_BACK
	};
	enum class BlendState
	{
		bsNONE,
	};

	PrimitiveTopology primitiveTopology = PrimitiveTopology::ptNONE;
	ComparisonFunc depthFunc = ComparisonFunc::cfNONE;
	ComparisonFunc stencilFunc = ComparisonFunc::cfNONE;
	bool depthStencilReadOnly = false;
	RasterizerState rasterizerState = RasterizerState::rsNONE;
	BlendState blendState = BlendState::bsNONE;

	std::wstring_view vertexShaderPath;
	std::wstring_view pixelShaderPath;

	bool IsComplete() const
	{
		bool complete = true;

		complete &= primitiveTopology != PrimitiveTopology::ptNONE;
		complete &= depthFunc != ComparisonFunc::cfNONE;
		complete &= stencilFunc != ComparisonFunc::cfNONE;
		complete &= rasterizerState != RasterizerState::rsNONE;
		complete &= blendState != BlendState::bsNONE;
		complete &= !vertexShaderPath.empty();
		complete &= !pixelShaderPath.empty();

		return complete;
	}
};