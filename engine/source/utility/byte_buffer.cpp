#include "byte_buffer.h"
#include <cassert>

namespace utility
{
	const ByteBuffer::Layout::Attribute& ByteBuffer::Layout::GetAttribute(size_t _index) const
	{
		return attributes_[_index];
	}

	size_t ByteBuffer::Layout::GetAttributeOffset(size_t _index) const
	{
		return GetAttribute(_index).offset_;
	}

	size_t ByteBuffer::Layout::GetAttributeSize(size_t _index) const
	{
		return GetAttribute(_index).size_;
	}

	size_t ByteBuffer::Layout::GetNumAttibutes() const
	{
		return attributes_.size();
	}

	size_t ByteBuffer::Layout::GetSizeInBytes() const
	{
		size_t totalSize = 0;
		for (const auto& attribute : attributes_)
		{
			totalSize += attribute.size_;
		}
		return totalSize;
	}

	ByteBuffer::Element::Element(const Layout& _layout, const uint8_t* _rawData)
		: layout_(_layout)
		, rawData_(_rawData)
	{
	}

	void ByteBuffer::SetLayout(const std::optional<Layout>& _layout)
	{
		layout_ = _layout;
	}

	std::optional<ByteBuffer::Layout> ByteBuffer::GetLayout() const
	{
		return layout_;
	}

	size_t ByteBuffer::GetElementSize() const
	{
		return layout_ ? layout_->GetSizeInBytes() : 0;
	}

	size_t ByteBuffer::GetNumElements() const
	{
		return layout_ ? rawBytes_.size() / layout_->GetSizeInBytes() : 0;
	}

	size_t ByteBuffer::GetSizeInBytes() const
	{
		return rawBytes_.size();
	}

	ByteBuffer::Element ByteBuffer::Add()
	{
		assert(layout_.has_value());

		const size_t size = rawBytes_.size();
		rawBytes_.resize(size + layout_->GetSizeInBytes());
		return Element(*layout_, rawBytes_.data() + size);
	}

	ByteBuffer::Element ByteBuffer::At(size_t _index) const
	{
		assert(layout_.has_value());

		return Element(*layout_, rawBytes_.data() + (layout_->GetSizeInBytes() * _index));
	}

	const uint8_t* ByteBuffer::GetRawBufferAddress() const
	{
		return rawBytes_.data();
	}
}