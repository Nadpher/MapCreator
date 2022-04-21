#pragma once

#include "pch.h"
#include "Tile.h"

#include <vector>

namespace nadpher
{

class Map : public sf::Drawable
{
public:
	void placeTile(const sf::Vector2f& position);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::vector<Tile> tiles_;
};

}

