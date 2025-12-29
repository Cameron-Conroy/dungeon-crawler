#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../game/RunState.hpp"
#include <SFML/Graphics.hpp>

class PlayingState;

class GameOverState : public GameState {
public:
    GameOverState(sf::Vector2f windowSize, const RunState& runState)
        : windowSize(windowSize), stats(runState) {}

    void enter() override {}
    void exit() override {}

    void update(float dt) override {}

    void render(sf::RenderWindow& window) override {
        // Dark overlay
        sf::RectangleShape overlay(windowSize);
        overlay.setFillColor(sf::Color(20, 0, 0, 200));
        window.draw(overlay);

        // Game Over box
        sf::RectangleShape box({300.f, 250.f});
        box.setPosition({windowSize.x / 2.f - 150.f, windowSize.y / 2.f - 125.f});
        box.setFillColor(sf::Color(40, 30, 30));
        box.setOutlineColor(sf::Color(150, 50, 50));
        box.setOutlineThickness(4.f);
        window.draw(box);

        // "GAME OVER" title
        sf::RectangleShape title({200.f, 40.f});
        title.setPosition({windowSize.x / 2.f - 100.f, windowSize.y / 2.f - 100.f});
        title.setFillColor(sf::Color(200, 60, 60));
        window.draw(title);

        // Stats
        float statY = windowSize.y / 2.f - 40.f;

        // Floor reached
        sf::RectangleShape floorStat({150.f, 25.f});
        floorStat.setPosition({windowSize.x / 2.f - 75.f, statY});
        floorStat.setFillColor(sf::Color(80, 80, 100));
        window.draw(floorStat);

        // Floor number bars
        for (int i = 0; i < stats.currentFloor; ++i) {
            sf::RectangleShape bar({15.f, 15.f});
            bar.setPosition({windowSize.x / 2.f + 30.f + i * 20.f, statY + 5.f});
            bar.setFillColor(sf::Color(200, 200, 100));
            window.draw(bar);
        }

        // Enemies killed
        sf::RectangleShape killStat({150.f, 25.f});
        killStat.setPosition({windowSize.x / 2.f - 75.f, statY + 35.f});
        killStat.setFillColor(sf::Color(100, 60, 60));
        window.draw(killStat);

        // Rooms visited
        sf::RectangleShape roomStat({150.f, 25.f});
        roomStat.setPosition({windowSize.x / 2.f - 75.f, statY + 70.f});
        roomStat.setFillColor(sf::Color(60, 80, 100));
        window.draw(roomStat);

        // Restart prompt
        sf::RectangleShape prompt({120.f, 30.f});
        prompt.setPosition({windowSize.x / 2.f - 60.f, windowSize.y / 2.f + 80.f});
        prompt.setFillColor(sf::Color(100, 120, 100));
        window.draw(prompt);
    }

    void handleEvent(const sf::Event& event) override;

private:
    sf::Vector2f windowSize;
    RunState stats;
};
