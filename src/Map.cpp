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
	for (const Tile& tile : tiles_)
	{
		target.draw(tile, states);
	}
}

void Map::placeTile(const std::string& texturePath, const sf::Vector2u& texturePosition, const sf::Vector2f& position)
{
	if (texturePath.empty())
	{
		spdlog::info("No tilesheet selected");
		return;
	}

	tiles_.push_back({ texturePath, sf::IntRect(sf::Vector2i(texturePosition), sf::Vector2i(tileSize_)), position });
}

}