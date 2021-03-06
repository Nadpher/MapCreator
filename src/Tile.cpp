#include "Tile.h"

#include "ResourceManager.h"

namespace nadpher
{

Tile::Tile(const std::string& texturePath, const sf::IntRect& textureRect, const sf::Vector2f& position)
{
	sprite_.setTexture(*ResourceManager<sf::Texture>::get(texturePath));
	sprite_.setPosition(position);
	sprite_.setTextureRect(textureRect);
}

void Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}

}