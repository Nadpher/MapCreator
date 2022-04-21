#pragma once

#include "pch.h"
#include <string>

namespace nadpher
{

class Tile : public sf::Drawable
{
public:
	Tile(const std::string& texturePath, const sf::Vector2f& position = { 0.0f, 0.0f });
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite sprite_;
};

}

