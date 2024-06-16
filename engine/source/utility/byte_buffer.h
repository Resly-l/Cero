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
				size_t size_ = 0;
				size_t offset_ = 0;
			};

		private:
			std::vector<Attribute> attributes_;

		public:
			Layout() = default;
			Layout(const Layout&) = default;
			Layout& operator=(const Layout&) = default;

		public:
			template<typename T>
			void AddAttribute();
			const Attribute& GetAttribute(size_t _index) const;

			size_t GetAttributeOffset(size_t _index) const;
			size_t GetAttributeSize(size_t _index) const;
			size_t GetNumAttibutes() const;
			size_t GetSizeInBytes() const;
		};

	private:
		class Element final
		{
		private:
			Layout layout_;
			const uint8_t* rawData_ = nullptr;

		public:
			Element(const Layout& _layout, const uint8_t* _rawData);

		public:
			template <typename T>
			T& Get(size_t _index);
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

		Element Add();
		Element At(size_t _index) const;

		const uint8_t* GetRawBufferAddress() const;
	};
}

namespace utility
{
	template <typename T>
	inline T& ByteBuffer::Element::Get(size_t _index)
	{
		return *(T*)(rawData_ + layout_.GetAttributeOffset(_index));
	}

	template<typename T>
	inline void ByteBuffer::Layout::AddAttribute()
	{
		Attribute attribute;
		attribute.offset_ = attributes_.empty() ? 0 : (attributes_.back().offset_ + attributes_.back().size_);
		attribute.size_ = sizeof(T);
		attributes_.push_back(attribute);
	}
}