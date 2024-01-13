#include "byte_buffer.h"

namespace utility
{
	const ByteBuffer::Layout::Attribute& ByteBuffer::Layout::GetAttribute(std::string_view _name) const
	{
		auto it = std::find_if(attributes_.begin(), attributes_.end(),
			[_name](const Attribute& _attribute)
			{
				return _attribute.name_ == _name;
			});

		if (it == attributes_.end())
		{
			throw std::exception("no attribute in layout");
		}

		return *it;
	}

	const ByteBuffer::Layout::Attribute& ByteBuffer::Layout::GetAttribute(size_t _index) const
	{
		return attributes_[_index];
	}

	size_t ByteBuffer::Layout::GetAttributeOffset(std::string_view _name) const
	{
		return GetAttribute(_name).offset_;
	}

	size_t ByteBuffer::Layout::GetAttributeOffset(size_t _index) const
	{
		return GetAttribute(_index).offset_;
	}

	size_t ByteBuffer::Layout::GetAttributeSize(std::string_view _name) const
	{
		return GetAttribute(_name).size_;
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

	std::optional<ByteBuffer::Element> ByteBuffer::Add()
	{
		if (!layout_)
		{
			return std::nullopt;
		}

		const size_t size = rawBytes_.size();
		rawBytes_.resize(size + layout_->GetSizeInBytes());
		return Element(*layout_, rawBytes_.data() + size);
	}

	std::optional<ByteBuffer::Element> ByteBuffer::At(size_t _index) const
	{
		if (!layout_)
		{
			return std::nullopt;
		}

		return Element(*layout_, rawBytes_.data() + (layout_->GetSizeInBytes() * _index));
	}

	const uint8_t* ByteBuffer::GetRawBufferAddress() const
	{
		return rawBytes_.data();
	}
}