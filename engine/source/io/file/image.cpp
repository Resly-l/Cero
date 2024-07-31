#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb/stb_image.h"

namespace io::file
{
	bool Image::Load(std::string_view _path)
	{
		int width, height, numChannels;
		stbi_uc* loaded = stbi_load(_path.data(), &width, &height, &numChannels, STBI_rgb_alpha);
		if (!loaded)
		{
			return false;
		}

		width_ = width;
		height_ = height;

		const size_t numPixels = size_t(width) * height;
		const size_t size = size_t(width) * height * numChannels;
		constexpr uint8_t maxColor = std::numeric_limits<uint8_t>::max();
		if (numChannels == 3)
		{
			colors_.reserve(numPixels * 4);
			for (int i = 0; i < size; i += 3)
			{
				colors_.push_back(loaded[i]);
				colors_.push_back(loaded[i + 1]);
				colors_.push_back(loaded[i + 2]);
				colors_.push_back(maxColor);
			}
		}
		else if (numChannels == 4)
		{
			colors_.resize(size);
			memcpy(colors_.data(), loaded, size);
		}

		stbi_image_free(loaded);

		return true;
	}

	bool Image::IsLoaded() const
	{
		return !colors_.empty();
	}

	uint8_t Image::At(uint32_t _x, uint32_t _y) const
	{
		size_t index = size_t(_x* width_) + _y;
		return (index >= colors_.size()) ? NULL : colors_[index];
	}
}