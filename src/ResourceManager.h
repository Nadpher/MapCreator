#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace nadpher
{

template <typename T>
class ResourceManager
{
public:
	static std::shared_ptr<T> get(const std::string& path);

private:
	static std::unordered_map<std::string, std::shared_ptr<T>> resources_;
};

}

