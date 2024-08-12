#pragma once
#include <string>
#include <iostream>

namespace utility
{
	class Log
	{
	private:
		struct Keyword
		{
		protected:
			const std::string keyword_;

		public:
			Keyword(const std::string& _keyword);
			std::string Get() const;
		};

	public:
		struct Category : public Keyword
		{
		public:
			Category(const std::string& _keyword);

			static Category graphics;
			static Category file;
		};

		struct Level : public Keyword
		{
		public:
			Level(const std::string& _keyword);

			static Level message;
			static Level warning;
			static Level error;
		};

	public:
		static std::string Format(Category _category, Level _level, const std::string& _message);
	};
}