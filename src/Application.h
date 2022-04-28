#pragma once

#include "pch.h"

#include "Map.h"

#include <string>

namespace nadpher
{

class Application
{
public:
	static int init(unsigned int width, unsigned int height, const std::string& title = "Map Editor");
	static void run();

private:

	static void handleEvents();
	static void zoomEvent(const sf::Event& event);
	static void buttonPressEvent(const sf::Event& event);

	static void drawHoveredCell();
	static void drawGUI();
	static void drawTileSelection();
	static void drawGrid();

	static void save();
	static void saveAs();
	static void open();

	static std::string tilesheet_;
	static sf::Vector2u selectedTile_;
	static std::string filePath_;
	static bool isPanning_;
	static float zoomLevel_;
	static sf::Vector2i cachedMousePosition_;

	static sf::RenderWindow window_;
	static sf::View view_;

	static Map map_;
};

}

