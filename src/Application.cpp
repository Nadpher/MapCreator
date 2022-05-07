#include "Application.h"
#include "ResourceManager.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <nfd.h>

namespace nadpher
{

bool Application::isPanning_ = false;
float Application::zoomLevel_ = 1.0f;
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
		return 0;
	}

	view_.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)));
	window_.setView(view_);

	map_.setTileSheet("res/tilesheet1.png");

	return 1;
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

	if (mousePosition.x > 0.0f)
	{
		mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
	}
	else
	{
		mousePosition.x -= static_cast<float>(tileSize.x) - std::fmodf(std::abs(mousePosition.x), static_cast<float>(tileSize.x));
	}

	if (mousePosition.y > 0.0f)
	{
		mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));
	}
	else
	{
		mousePosition.y -= static_cast<float>(tileSize.y) - std::fmodf(std::abs(mousePosition.y), static_cast<float>(tileSize.y));
	}

	cell.setPosition(mousePosition);

	window_.draw(cell);
}

void Application::drawGrid()
{
	sf::VertexArray arr(sf::LinesStrip, 2);

	// grey color
	arr[0].color = sf::Color(75, 75, 75, 255);
	arr[1].color = sf::Color(75, 75, 75, 255);

	// just fuck it
	constexpr float multiplier = 1000.0f;
	sf::Vector2u tileSize = map_.getTileSize();
	float xMin = static_cast<float>(tileSize.x) * -multiplier;
	float xMax = static_cast<float>(tileSize.x) * multiplier;

	float yMin = static_cast<float>(tileSize.y) * -multiplier;
	float yMax = static_cast<float>(tileSize.y) * multiplier;

	// columns
	float pos = xMin; // -64000.0f

	for ( ; pos < 0.0f; pos += tileSize.x)
	{
		arr[0].position.x = pos;
		arr[0].position.y = yMin;

		arr[1].position.x = pos;
		arr[1].position.y = yMax;
		window_.draw(arr);
	}

	arr[0].position = sf::Vector2f(0.0f, yMin);
	arr[1].position = sf::Vector2f(0.0f, yMax);

	arr[0].color = sf::Color::White;
	arr[1].color = sf::Color::White;
	window_.draw(arr);

	// grey color
	arr[0].color = sf::Color(75, 75, 75, 255);
	arr[1].color = sf::Color(75, 75, 75, 255);

	pos = tileSize.x;
	for ( ; pos <= xMax; pos += tileSize.x)
	{
		arr[0].position.x = pos;
		arr[0].position.y = yMin;

		arr[1].position.x = pos;
		arr[1].position.y = yMax;
		window_.draw(arr);
	}

	// rows
	pos = yMin;
	for (; pos < 0.0f; pos += tileSize.y)
	{
		arr[0].position.x = xMin;
		arr[0].position.y = pos;

		arr[1].position.x = xMax;
		arr[1].position.y = pos;
		window_.draw(arr);
	}

	arr[0].position = sf::Vector2f(xMin, 0.0f);
	arr[1].position = sf::Vector2f(xMax, 0.0f);

	arr[0].color = sf::Color::White;
	arr[1].color = sf::Color::White;
	window_.draw(arr);

	// grey color
	arr[0].color = sf::Color(75, 75, 75, 255);
	arr[1].color = sf::Color(75, 75, 75, 255);

	// rows
	pos = tileSize.y;
	for (; pos <= xMax; pos += tileSize.x)
	{
		arr[0].position.x = xMin;
		arr[0].position.y = pos;

		arr[1].position.x = xMax;
		arr[1].position.y = pos;
		window_.draw(arr);
	}
}

void Application::drawGUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save As"))
			{
				saveAs();
			}

			if (ImGui::MenuItem("Save"))
			{
				save();
			}

			if (ImGui::MenuItem("Open"))
			{
				open();
			}

			if (ImGui::MenuItem("Quit"))
			{
				window_.close();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	drawTileSelection();
}

void Application::open()
{
	nfdchar_t* path = nullptr;
	nfdresult_t result = NFD_OpenDialog("json", NULL, &path);

	if (result == NFD_OKAY)
	{
		map_.deserialize(path);
	}
	else if (result != NFD_CANCEL)
	{
		spdlog::error(NFD_GetError());
	}

	std::free(path);
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

	const sf::Vector2u textureSize = (*ResourceManager<sf::Texture>::get(map_.getTileSheet())).getSize();
	const sf::Vector2u tileSize = map_.getTileSize();
	sf::Vector2u tiles = sf::Vector2u(textureSize.x / tileSize.x, textureSize.y / tileSize.y);

	if (ImGui::BeginTable("Tiles", tiles.x, 
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_SizingFixedSame))
	{
		sf::Sprite tile;
		tile.setTexture(*ResourceManager<sf::Texture>::get(map_.getTileSheet()));

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

		if (mousePosition.x > 0.0f)
		{
			mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
		}
		else
		{
			mousePosition.x -= static_cast<float>(tileSize.x) - std::fmodf(std::abs(mousePosition.x), static_cast<float>(tileSize.x));
		}

		if (mousePosition.y > 0.0f)
		{
			mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));
		}
		else
		{
			mousePosition.y -= static_cast<float>(tileSize.y) - std::fmodf(std::abs(mousePosition.y), static_cast<float>(tileSize.y));
		}
		
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

		if (mousePosition.x > 0.0f)
		{
			mousePosition.x -= std::fmodf(mousePosition.x, static_cast<float>(tileSize.x));
		}
		else
		{
			mousePosition.x -= static_cast<float>(tileSize.x) - std::fmodf(std::abs(mousePosition.x), static_cast<float>(tileSize.x));
		}

		if (mousePosition.y > 0.0f)
		{
			mousePosition.y -= std::fmodf(mousePosition.y, static_cast<float>(tileSize.y));
		}
		else
		{
			mousePosition.y -= static_cast<float>(tileSize.y) - std::fmodf(std::abs(mousePosition.y), static_cast<float>(tileSize.y));
		}
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