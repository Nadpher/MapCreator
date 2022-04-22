#include "Application.h"

#include <imgui.h>
#include <imgui-SFML.h>

namespace nadpher
{

bool Application::isPanning_ = false;
bool Application::isDrawGrid_ = false;
float Application::zoomLevel_ = 1.0f;
sf::Vector2i Application::cachedMousePosition_;
sf::RenderWindow Application::window_;
sf::View Application::view_;
Map Application::map_;

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

		window_.setView(view_);
		window_.clear();

		window_.draw(map_);
		if (isDrawGrid_)
		{
			drawGrid();
		}
		ImGui::SFML::Render(window_);

		window_.display();
	}

	ImGui::SFML::Shutdown();
}

void Application::drawGrid()
{
	// 64 is the px size of the test tile
	// needs to be able to be set from gui
	int tileSize = 64 / zoomLevel_;
	sf::Vector2u windowSize = window_.getSize();
	int xTiles = windowSize.x / tileSize;
	int yTiles = windowSize.y / tileSize;

	sf::VertexArray arr(sf::LinesStrip, 2);

	for (int i = 1; i < xTiles; ++i)
	{
		arr[0].position = window_.mapPixelToCoords({ i * tileSize, 0 });
		arr[1].position = window_.mapPixelToCoords({ i * tileSize, (int)windowSize.y });
		window_.draw(arr);
	}

	for (int i = 1; i < yTiles; ++i)
	{
		arr[0].position = window_.mapPixelToCoords({ 0, i * tileSize });
		arr[1].position = window_.mapPixelToCoords({ (int)windowSize.x, i * tileSize });
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

	if (ImGui::Begin("Options"))
	{
		ImGui::Checkbox("Show grid", &isDrawGrid_);

		ImGui::End();
	}

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
}


}