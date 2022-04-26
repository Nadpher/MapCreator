#pragma once

#include "pch.h"
#include "Tile.h"
#include "Coord.h"

#include <map>

namespace nadpher
{

class Map : public sf::Drawable
{
public:
	Map(const sf::Vector2u& tileSize);
	Map(unsigned int tileWidth, unsigned int tileHeight);

	sf::Vector2u getTileSize() const { return tileSize_; }
	void setTileSheet(const std::string& texturePath) { tilesheet_ = texturePath; }

	void serialize(const std::string& filePath);
	void deserialize(const std::string& filePath);

	void placeTile(const sf::Vector2u& texturePosition, const sf::Vector2f& position);
	void eraseTile(const sf::Vector2f& position);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

	std::string tilesheet_;
	sf::Vector2u tileSize_;
	std::map<Coord, Tile> tiles_;
};

}

