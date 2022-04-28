#include "Application.h"
#include "ResourceManager.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <nfd.h>

namespace nadpher
{

bool Application::isPanning_ = false;
float Application::zoomLevel_ = 1.0f;
std::string Application::tilesheet_ = "res/tilesheet1.png";
std::string Application::filePath_;
sf::Vector2u Application::selectedTile_;
sf::Vector2i Application::cachedMousePosition_;
sf::RenderWindow Application::window_;
sf::View Application::view_;
Map Application::map_(64, 64); // temporary

int Application::init(unsigned int width, unsigned int height, const std::string& title)
{
	window_.create(sf::VideoMode(width, height), title);

	if (!window_.isOpen())
	{
		spdlog::error("Couldn't create SFML window.");
		return 1;
	}

	view_.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)));
	window_.setView(view_);

	map_.setTileSheet("res/tilesheet1.png");

	return 0;
}

void Application::run()
{
	ImGui::SFML::Init(window_);

	sf::Clock clock;
	while (window_.isOpen())
	{
		sf::Time elapsed = clock.restart();
		
		handleEvents();

		ImGui::SFML::Update(window_, elapsed);

		drawGUI();
		
		window_.clear();

		window_.draw(map_);
		drawGrid();
		drawHoveredCell();
		ImGui::SFML::Render(window_);

		window_.display();
	}

	ImGui::SFML::Shutdown();
}

void Application::drawHoveredCell()
{
	sf::Vector2u tileSize = map_.getTileSize();

	sf::RectangleShape cell(sf::Vector2f(static_cast<float>(tileSize.x),
										 static_cast<float>(tileSize.y)));
	cell.setFillColor(sf::Color::Transparent);
	cell.setOutlineColor(sf::Color::Red);
	cell.setOutlineThickness(2.0f);

	sf::Vector2f mousePosition = window_.mapPixelToCoords(sf::Mouse::getPosition(window_), view_);

	mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
	mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));

	cell.setPosition(mousePosition);

	window_.draw(cell);
}

void Application::drawGrid()
{
	sf::VertexArray arr(sf::LinesStrip, 2);
	sf::Vector2f center = view_.getCenter();
	sf::Vector2u windowSize = window_.getSize();

	sf::Vector2u tileSize = map_.getTileSize();
	const int xScaledTileSize = tileSize.x / zoomLevel_;
	const int yScaledTileSize = tileSize.y / zoomLevel_;

	int xTiles = windowSize.x / xScaledTileSize;
	int yTiles = windowSize.y / yScaledTileSize;

	// temporary grey color
	arr[0].color = sf::Color(150, 150, 150, 255);
	arr[1].color = sf::Color(150, 150, 150, 255);

	// columns
	float offset = fmodf(center.x, static_cast<float>(tileSize.x));
	for (int i = 0; i <= xTiles; ++i)
	{
		arr[0].position = window_.mapPixelToCoords(sf::Vector2i(i * xScaledTileSize, 0), view_);
		arr[0].position.x -= offset;

		arr[1].position = window_.mapPixelToCoords(sf::Vector2i(i * xScaledTileSize, windowSize.y), view_);
		arr[1].position.x -= offset;
		window_.draw(arr);
	}

	// rows
	offset = fmodf(center.y, static_cast<float>(tileSize.y));
	for (int i = 0; i <= yTiles; ++i)
	{
		arr[0].position = window_.mapPixelToCoords(sf::Vector2i(0, i * yScaledTileSize), view_);
		arr[0].position.y -= offset;

		arr[1].position = window_.mapPixelToCoords(sf::Vector2i(windowSize.x, i * yScaledTileSize), view_);
		arr[1].position.y -= offset;
		window_.draw(arr);
	}

}

void Application::drawGUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save As", "CTRL+F2"))
			{
				saveAs();
			}

			if (ImGui::MenuItem("Save", "F2"))
			{
				save();
			}

			if (ImGui::MenuItem("Open", "F3"))
			{
				map_.deserialize(filePath_);
			}

			if (ImGui::MenuItem("Quit", "CTRL+Q"))
			{
				window_.close();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	drawTileSelection();
}

void Application::saveAs()
{
	nfdchar_t* path = nullptr;
	nfdresult_t result = NFD_SaveDialog("json", NULL, &path);

	if (result == NFD_OKAY)
	{
		filePath_ = std::string(path);
		map_.serialize(filePath_);
	}
	else if (result != NFD_CANCEL)
	{
		spdlog::error(NFD_GetError());
	}

	std::free(path);
}

void Application::save()
{
	if (filePath_.empty())
	{
		saveAs();
	}
	else
	{
		map_.serialize(filePath_);
	}
}

void Application::drawTileSelection()
{
	ImGui::Begin("Tiles");

	const sf::Vector2u textureSize = (*ResourceManager<sf::Texture>::get(tilesheet_)).getSize();
	const sf::Vector2u tileSize = map_.getTileSize();
	sf::Vector2u tiles = sf::Vector2u(textureSize.x / tileSize.x, textureSize.y / tileSize.y);

	if (ImGui::BeginTable("Tiles", tiles.x, 
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_SizingFixedSame))
	{
		sf::Sprite tile;
		tile.setTexture(*ResourceManager<sf::Texture>::get(tilesheet_));

		for (unsigned int i = 0; i < tiles.y; ++i)
		{
			ImGui::TableNextRow();
			for (unsigned int j = 0; j < tiles.x; ++j)
			{
				ImGui::TableNextColumn();
				tile.setTextureRect(sf::IntRect(sf::Vector2i(j * tileSize.x, i * tileSize.y), sf::Vector2i(tileSize)));

				ImGui::PushID(i * tiles.x + j);
				if (ImGui::ImageButton(tile, 0))
				{
					selectedTile_ = sf::Vector2u(j * tileSize.x, i * tileSize.y);
				}
				ImGui::PopID();
				
			}
		}

		ImGui::EndTable();
	}


	ImGui::End();
}

void Application::handleEvents()
{
	sf::Event event;
	while (window_.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		switch (event.type)
		{
		case sf::Event::Closed:
			window_.close();
			break;

		case sf::Event::MouseButtonPressed:
			buttonPressEvent(event);
			break;

		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Button::Middle)
			{
				isPanning_ = false;
			}
			break;

		case sf::Event::MouseMoved:
			if (isPanning_)
			{
				view_.move((cachedMousePosition_.x - event.mouseMove.x) * zoomLevel_,
						   (cachedMousePosition_.y - event.mouseMove.y) * zoomLevel_);

				cachedMousePosition_ = sf::Mouse::getPosition(window_);
				window_.setView(view_);
			}
			break;

		case sf::Event::MouseWheelScrolled:
			zoomEvent(event);
			break;

		default:
			break;
		}
	}
}

void Application::buttonPressEvent(const sf::Event& event)	
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	sf::Vector2i pixelPosition = sf::Mouse::getPosition(window_);
	sf::Vector2f mousePosition = window_.mapPixelToCoords(pixelPosition, view_);
	if (event.mouseButton.button == sf::Mouse::Button::Left)
	{
		sf::Vector2u tileSize = map_.getTileSize();

		mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
		mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));
		map_.placeTile(selectedTile_, mousePosition);
	}
	else if (event.mouseButton.button == sf::Mouse::Button::Middle)
	{
		isPanning_ = true;
		cachedMousePosition_ = pixelPosition;
	}
	else if (event.mouseButton.button == sf::Mouse::Button::Right)
	{
		sf::Vector2u tileSize = map_.getTileSize();

		mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
		mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));
		map_.eraseTile(mousePosition);
	}
}

void Application::zoomEvent(const sf::Event& event)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	constexpr float timesTwo = 2.0f;
	constexpr float half = 0.5f;

	// scroll up
	if (event.mouseWheelScroll.delta > 0)
	{
		view_.zoom(half);
		zoomLevel_ *= half;
	}
	// scroll down
	else
	{
		view_.zoom(timesTwo);
		zoomLevel_ *= timesTwo;
	}
	window_.setView(view_);
}


}