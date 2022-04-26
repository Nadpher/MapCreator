#include "Map.h"

#include <fstream>
#include <nlohmann/json.hpp>

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

void Map::serialize(const std::string& texturePath)
{
	std::ofstream out("test.json", std::ios::out);
	if (out.is_open())
	{
		nlohmann::json json;

		json["tilesheet"] = texturePath;

		json["tileWidth"] = tileSize_.x;
		json["tileHeight"] = tileSize_.y;

		size_t len = tiles_.size();

		int i = 0;
		for (auto it = tiles_.begin(); it != tiles_.end(); ++it, ++i)
		{
			sf::IntRect textureRect = it->second.sprite_.getTextureRect();
			sf::Vector2f position = it->second.sprite_.getPosition();

			json["tiles"][i]["xTexturePosition"] = textureRect.left;
			json["tiles"][i]["yTexturePosition"] = textureRect.top;

			json["tiles"][i]["xPosition"] = position.x;
			json["tiles"][i]["yPosition"] = position.y;
		}

		out << json;
	}
	else
	{
		spdlog::error("Couldn't open json file for serialization");
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

	tiles_.erase(index);
}

}