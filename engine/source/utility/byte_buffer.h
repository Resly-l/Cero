#pragma once
#include "utility/stl.h"

namespace utility
{
	class ByteBuffer
	{
	public:
		class Layout
		{
		private:
			struct Attribute final
			{
				std::string name_;
				size_t size_ = 0;
				size_t offset_ = 0;
			};

		private:
			std::vector<Attribute> attributes_;

		public:
			template<typename T>
			bool AddAttribute(std::string_view _name);
			const Attribute& GetAttribute(std::string_view _name) const;
			const Attribute& GetAttribute(size_t _index) const;

			size_t GetAttributeOffset(std::string_view _name) const;
			size_t GetAttributeOffset(size_t _index) const;
			size_t GetAttributeSize(std::string_view _name) const;
			size_t GetAttributeSize(size_t _index) const;
			size_t GetNumAttibutes() const;
			size_t GetSizeInBytes() const;
		};

	private:
		class Element final
		{
		private:
			const Layout layout_;
			const uint8_t* rawData_ = nullptr;

		public:
			Element(const Layout& _layout, const uint8_t* _rawData);

		public:
			template <typename ReturnTy>
			ReturnTy& Get(std::string_view _element);
		};

	private:
		std::optional<Layout> layout_;
		std::vector<uint8_t> rawBytes_;

	public:
		void SetLayout(const std::optional<Layout>& _layout);
		std::optional<Layout> GetLayout() const;

		size_t GetElementSize() const;
		size_t GetNumElements() const;
		size_t GetSizeInBytes() const;

		std::optional<Element> Add();
		std::optional<Element> At(size_t _index) const;

		const uint8_t* GetRawBufferAddress() const;
	};
}

namespace utility
{
	template <typename ReturnTy>
	inline ReturnTy& ByteBuffer::Element::Get(std::string_view _attributeName)
	{
		if (layout_.GetAttributeSize(_attributeName) != sizeof(ReturnTy))
		{
			throw std::exception("attribute size mismatch");
		}

		return *(ReturnTy*)(rawData_ + layout_.GetAttributeOffset(_attributeName));
	}

	template<typename T>
	inline bool ByteBuffer::Layout::AddAttribute(std::string_view _name)
	{
		auto it = std::find_if(attributes_.begin(), attributes_.end(),
			[&_name](const Attribute& _attribute)
			{
				return _attribute.name_ == _name;
			});

		if (it != attributes_.end())
		{
			return false;
		}

		Attribute attribute;
		attribute.name_ = _name;
		attribute.offset_ = attributes_.empty() ? 0 : (attributes_.back().offset_ + attributes_.back().size_);
		attribute.size_ = sizeof(T);
		attributes_.push_back(attribute);

		return true;
	}
}