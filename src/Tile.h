#pragma once

#include "pch.h"
#include <string>

namespace nadpher
{

class Tile : public sf::Drawable
{
public:

	Tile() : sprite_() {}
	Tile(const std::string& texturePath, const sf::IntRect& textureRect, const sf::Vector2f& position);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite sprite_;
};

}

