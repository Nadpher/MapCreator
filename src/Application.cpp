#include "Application.h"

#include <imgui-SFML.h>

namespace nadpher
{

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

		window_.setView(view_);
		window_.clear();

		window_.draw(map_);
		ImGui::SFML::Render(window_);

		window_.display();
	}

	ImGui::SFML::Shutdown();
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
			if (event.mouseButton.button == sf::Mouse::Button::Left)
			{
				sf::Vector2i pixelPosition = sf::Mouse::getPosition(window_);
				map_.placeTile(window_.mapPixelToCoords(pixelPosition, view_));
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

void Application::zoomEvent(const sf::Event& event)
{
	constexpr float timesTwo = 2.0f;
	constexpr  float half = 0.5f;

	// scroll up
	if (event.mouseWheelScroll.delta > 0)
	{
		view_.zoom(half);
	}
	// scroll down
	else
	{
		view_.zoom(timesTwo);
	}
}


}