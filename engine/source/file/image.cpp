#include "image.h"
#include <string_view>
#include "utility/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image.h"

using utility::Log;

namespace file
{
	bool Image::Load(std::string_view _path)
	{
		int width, height, numChannels;
		stbi_uc* loaded = stbi_load(_path.data(), &width, &height, &numChannels, STBI_rgb_alpha);
		if (!loaded)
		{
			std::cout << Log::Format(Log::Category::file, Log::Level::warning, "failed to load image, path : " + std::string(_path.data()));
			return false;
		}

		width_ = width;
		height_ = height;

		const size_t numPixels = size_t(width) * height;
		const size_t size = size_t(width) * height * numChannels;
		constexpr uint8_t maxColor = std::numeric_limits<uint8_t>::max();
		
		colors_.resize(numPixels * 4);
		memcpy(colors_.data(), loaded, colors_.size());

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