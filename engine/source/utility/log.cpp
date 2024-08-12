#include "log.h"

namespace utility
{
	Log::Category Log::Category::graphics("graphics");
	Log::Category Log::Category::file("file");

	Log::Level Log::Level::message("message");
	Log::Level Log::Level::warning("warning");
	Log::Level Log::Level::error("error");

	std::string Log::Keyword::Get() const
	{
		return "[" + keyword_ + "]";
	}

	Log::Keyword::Keyword(const std::string& _keyword)
		: keyword_(_keyword)
	{}

	Log::Category::Category(const std::string& _keyword)
		: Log::Keyword(_keyword)
	{}

	Log::Level::Level(const std::string& _keyword)
		: Log::Keyword(_keyword)
	{}


	std::string Log::Format(Category _category, Level _level, const std::string& _message)
	{
		return _category.Get() + _level.Get() + " " + _message;
	}
}