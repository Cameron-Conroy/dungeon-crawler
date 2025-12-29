#pragma once

#include "core/StateManager.hpp"
#include "core/AssetManager.hpp"
#include "states/MainMenuState.hpp"
#include <SFML/Graphics.hpp>

class Application {
public:
    Application()
        : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Dungeon Crawler")
    {
        window.setFramerateLimit(60);
        loadAssets();
        stateManager.push(std::make_unique<MainMenuState>(
            sf::Vector2f{static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)}
        ));
    }

public:
    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();

            processEvents();

            if (stateManager.empty()) {
                window.close();
                continue;
            }

            stateManager.update(dt);

            window.clear();
            stateManager.render(window);
            window.display();
        }
    }

private:
    void loadAssets() {
        AssetManager::instance().loadFont("pixel", "assets/fonts/PressStart2P-Regular.ttf");
    }

    void processEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }

            stateManager.handleEvent(*event);
        }
    }

    sf::RenderWindow window;
    StateManager stateManager;

    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
};
