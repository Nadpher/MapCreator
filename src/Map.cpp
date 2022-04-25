#include "Map.h"

namespace nadpher
{

Map::Map(unsigned int width, unsigned int height)
	: tileSize_(width, height)
{

}

Map::Map(const sf::Vector2u& tileSize)
	: tileSize_(tileSize)
{

}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto it = tiles_.begin(); it != tiles_.end(); ++it)
	{
		target.draw(it->second);
	}
}

void Map::placeTile(const std::string& texturePath, const sf::Vector2u& texturePosition, const sf::Vector2f& position)
{
	if (texturePath.empty())
	{
		spdlog::info("No tilesheet selected");
		return;
	}

	// i hate warnings
	Coord index = { static_cast<int>(position.x) / static_cast<int>(tileSize_.x),
					static_cast<int>(position.y) / static_cast<int>(tileSize_.y) };

	tiles_[index] = Tile(texturePath, sf::IntRect(sf::Vector2i(texturePosition), sf::Vector2i(tileSize_)), position);
}

void Map::eraseTile(const sf::Vector2f& position)
{
	Coord index = { static_cast<int>(position.x) / static_cast<int>(tileSize_.x),
					static_cast<int>(position.y) / static_cast<int>(tileSize_.y) };

	tiles_[index] = {};
}

}