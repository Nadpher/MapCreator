#include "ResourceManager.h"

#include "pch.h"

namespace nadpher
{

template <typename T>
std::unordered_map<std::string, std::shared_ptr<T>> ResourceManager<T>::resources_;

template <>
std::shared_ptr<sf::Texture> ResourceManager<sf::Texture>::get(const std::string& path)
{
	if (!resources_.count(path))
	{
		sf::Texture texture;
		if (!texture.loadFromFile(path))
		{
			spdlog::error("Couldn't load texture: {}", path);
			return nullptr;
		}
		
		resources_[path] = std::make_shared<sf::Texture>(texture);
	}
	
	return resources_[path];
}

}