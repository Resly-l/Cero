#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>
#include <functional>
#include <queue>
#include <condition_variable>
#include <optional>

class Dependency : public std::enable_shared_from_this<Dependency>
{
public:
	std::shared_ptr<class Dependent> dependent_;
	int value_ = 123;

public:
	void Initialize();
};

class Dependent
{
private:
	std::shared_ptr<Dependency> dependency_;

public:
	Dependent(std::shared_ptr<Dependency> _dependency)
		: dependency_(_dependency)
	{

	}
};

void Dependency::Initialize()
{
	dependent_ = std::make_shared<Dependent>(shared_from_this());
}

int main()
{
	auto dependency_ = std::make_shared<Dependency>();

	dependency_->Initialize();
	dependency_->value_ = 999;

	return 0;
}