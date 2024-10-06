#pragma once
#include "common.h"
#include "utility/byte_buffer.h"
#include "utility/forward_declaration.h"
#include "shader.h"
#include <string_view>
#include <unordered_map>

namespace graphics
{
	class Pipeline
	{
	public:
		struct Layout
		{
			PrimitiveTopology primitiveTopology_ = PrimitiveTopology::NONE;
			ComparisonFunc depthFunc_ = ComparisonFunc::NONE;
			ComparisonFunc stencilFunc_ = ComparisonFunc::NONE;
			bool depthStencilReadOnly_ = false;
			RasterizerState rasterizerState_ = RasterizerState::NONE;
			BlendState blendState_ = BlendState::NONE;
			Viewport viewport_;
			std::wstring_view vertexShaderPath_;
			std::wstring_view pixelShaderPath_;
			utility::ByteBuffer::Layout vertexInputLayout_;
			ShaderDescriptor descriptor_;
		};

	protected:
		std::unordered_map<uint32_t, std::pair<std::shared_ptr<ShaderBinding>, ShaderStage>> shaderBindings_;

	public:
		virtual ~Pipeline() {};

		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height) const = 0;

		void BindShaderBinding(std::shared_ptr<ShaderBinding> _shaderBinding, ShaderStage _stage, uint32_t _slot)
		{
			shaderBindings_[_slot] = { _shaderBinding, _stage};
		}

		/*
		* void BindShaderBindiing(Parent _parent);
		* std::map<uint32_t, std::shared_ptr<ShaderBinding>> bindings_;
		* 
		* void UpdateDescriptorSets(VkDescriptorSet _descriptorSet);
		* {
		*	std::vector<VkWriteDescriptorSet> descriptorWrites;
		*	for (size_t i = 0; i < bindings_.size(); i++)
		*	{
		*		VkWriteDescriptorSet descriptorWrite = std::static_pointer_cast<>(binding->GetSomethging())->GetDescriptorWrite();
		*		descriptorWrite.dstSet = _descriptorSet;
		*		descriptorWrite.dstBinding = (uint32_t)i;
		*		descriptorWrite.dstArrayElement = 0;
		*		descriptorWrite.descriptorCount = 0;
		*		descriptorWrites.push_back(descriptorWrite);
		*	}
		*	
		*	vkUpdateDesctiprotSets();
		* }
		* 
		*/
	};
}