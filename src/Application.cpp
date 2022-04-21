#include "Application.h"

#include <imgui-SFML.h>

namespace nadpher
{

bool Application::isPanning_ = false;
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
				view_.move(cachedMousePosition_.x - event.mouseMove.x,
						   cachedMousePosition_.y - event.mouseMove.y);

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