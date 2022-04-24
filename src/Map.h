#pragma once

#include "pch.h"
#include "Tile.h"

#include <vector>

namespace nadpher
{

class Map : public sf::Drawable
{
public:
	Map(const sf::Vector2u& tileSize);
	Map(unsigned int tileWidth, unsigned int tileHeight);

	sf::Vector2u getTileSize() const { return tileSize_; }

	void placeTile(const sf::Vector2f& position);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

	sf::Vector2u tileSize_;
	std::vector<Tile> tiles_;
};

}

