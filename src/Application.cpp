#include "Application.h"

#include <imgui.h>
#include <imgui-SFML.h>

namespace nadpher
{

bool Application::isPanning_ = false;
float Application::zoomLevel_ = 1.0f;
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

	sf::RectangleShape cell(sf::Vector2f(tileSize.x, tileSize.y));
	cell.setFillColor(sf::Color::Transparent);
	cell.setOutlineColor(sf::Color::Red);
	cell.setOutlineThickness(2.0f);

	sf::Vector2f mousePosition = window_.mapPixelToCoords(sf::Mouse::getPosition(window_), view_);

	mousePosition.x -= std::fmodf(mousePosition.x, tileSize.x);
	mousePosition.y -= std::fmodf(mousePosition.y, tileSize.y);

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

	unsigned int xTiles =  windowSize.x / xScaledTileSize;
	unsigned int yTiles = windowSize.y / yScaledTileSize;

	// temporary grey color
	arr[0].color = sf::Color(150, 150, 150, 255);
	arr[1].color = sf::Color(150, 150, 150, 255);

	// columns
	float offset = fmodf(center.x, tileSize.x);
	for (int i = 0; i <= xTiles; ++i)
	{
		arr[0].position = window_.mapPixelToCoords(sf::Vector2i(i * xScaledTileSize, 0), view_);
		arr[0].position.x -= offset;

		arr[1].position = window_.mapPixelToCoords(sf::Vector2i(i * xScaledTileSize, windowSize.y), view_);
		arr[1].position.x -= offset;
		window_.draw(arr);
	}

	// rows
	offset = fmodf(center.y, tileSize.y);
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
			if (ImGui::MenuItem("Open", "F2"))
			{
				spdlog::debug("Opened new file");
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::Begin("Options");
	
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

		mousePosition.x -= std::fmodf(mousePosition.x, tileSize.x);
		mousePosition.y -= std::fmodf(mousePosition.y, tileSize.y);
		map_.placeTile(mousePosition);
	}
	else if (event.mouseButton.button == sf::Mouse::Button::Middle)
	{
		isPanning_ = true;
		cachedMousePosition_ = pixelPosition;
	}
}

void Application::zoomEvent(const sf::Event& event)
{
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