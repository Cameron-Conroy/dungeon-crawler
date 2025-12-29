#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include <SFML/Graphics.hpp>

class PlayingState;

class MainMenuState : public GameState {
public:
    explicit MainMenuState(sf::Vector2f windowSize) : windowSize(windowSize) {}

    void enter() override {}
    void exit() override {}

    void update(float dt) override {
        pulseTimer += dt;
    }

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(30, 30, 40));

        // Title
        sf::RectangleShape title({300.f, 60.f});
        title.setPosition({windowSize.x / 2.f - 150.f, 150.f});
        title.setFillColor(sf::Color(80, 120, 80));
        window.draw(title);

        // "DUNGEON" text approximation with rectangles
        float startX = windowSize.x / 2.f - 100.f;
        float y = 165.f;
        for (int i = 0; i < 7; ++i) {
            sf::RectangleShape letter({20.f, 30.f});
            letter.setPosition({startX + i * 30.f, y});
            letter.setFillColor(sf::Color(200, 220, 200));
            window.draw(letter);
        }

        // Pulsing "Press Enter" indicator
        float alpha = 128.f + 127.f * std::sin(pulseTimer * 3.f);
        sf::RectangleShape prompt({200.f, 40.f});
        prompt.setPosition({windowSize.x / 2.f - 100.f, 350.f});
        prompt.setFillColor(sf::Color(100, 150, 100, static_cast<std::uint8_t>(alpha)));
        window.draw(prompt);

        // Arrow indicators
        sf::RectangleShape arrow({20.f, 20.f});
        arrow.setPosition({windowSize.x / 2.f - 10.f, 400.f});
        arrow.setFillColor(sf::Color(150, 150, 180));
        window.draw(arrow);
    }

    void handleEvent(const sf::Event& event) override;

private:
    sf::Vector2f windowSize;
    float pulseTimer = 0.f;
};
