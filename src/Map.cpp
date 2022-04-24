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

void Map::placeTile(const sf::Vector2f& position)
{
	tiles_.push_back({ "res/tile1.png", position });
}

}