#include "Map.h"

namespace nadpher
{

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