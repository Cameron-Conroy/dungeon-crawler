#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../game/RunState.hpp"
#include <SFML/Graphics.hpp>

class PlayingState;

class VictoryState : public GameState {
public:
    VictoryState(sf::Vector2f windowSize, const RunState& runState)
        : windowSize(windowSize), stats(runState) {}

    void enter() override {}
    void exit() override {}

    void update(float dt) override {
        celebrationTimer += dt;
    }

    void render(sf::RenderWindow& window) override {
        // Golden overlay
        float pulse = 0.5f + 0.3f * std::sin(celebrationTimer * 4.f);
        sf::RectangleShape overlay(windowSize);
        overlay.setFillColor(sf::Color(50, 50, 20, static_cast<std::uint8_t>(180 * pulse)));
        window.draw(overlay);

        // Victory box
        sf::RectangleShape box({320.f, 280.f});
        box.setPosition({windowSize.x / 2.f - 160.f, windowSize.y / 2.f - 140.f});
        box.setFillColor(sf::Color(40, 45, 30));
        box.setOutlineColor(sf::Color(200, 200, 100));
        box.setOutlineThickness(4.f);
        window.draw(box);

        // "VICTORY" title
        sf::RectangleShape title({200.f, 50.f});
        title.setPosition({windowSize.x / 2.f - 100.f, windowSize.y / 2.f - 115.f});
        title.setFillColor(sf::Color(220, 200, 80));
        window.draw(title);

        // Stats
        float statY = windowSize.y / 2.f - 40.f;

        // Floors cleared
        sf::RectangleShape floorStat({180.f, 25.f});
        floorStat.setPosition({windowSize.x / 2.f - 90.f, statY});
        floorStat.setFillColor(sf::Color(100, 100, 60));
        window.draw(floorStat);

        for (int i = 0; i < stats.currentFloor; ++i) {
            sf::RectangleShape star({20.f, 20.f});
            star.setPosition({windowSize.x / 2.f + 40.f + i * 25.f, statY + 2.f});
            star.setFillColor(sf::Color(255, 220, 100));
            window.draw(star);
        }

        // Enemies defeated
        sf::RectangleShape killStat({180.f, 25.f});
        killStat.setPosition({windowSize.x / 2.f - 90.f, statY + 35.f});
        killStat.setFillColor(sf::Color(100, 70, 70));
        window.draw(killStat);

        // Rooms explored
        sf::RectangleShape roomStat({180.f, 25.f});
        roomStat.setPosition({windowSize.x / 2.f - 90.f, statY + 70.f});
        roomStat.setFillColor(sf::Color(70, 90, 110));
        window.draw(roomStat);

        // Pickups collected
        sf::RectangleShape pickupStat({180.f, 25.f});
        pickupStat.setPosition({windowSize.x / 2.f - 90.f, statY + 105.f});
        pickupStat.setFillColor(sf::Color(110, 70, 90));
        window.draw(pickupStat);

        // Play again prompt
        sf::RectangleShape prompt({140.f, 35.f});
        prompt.setPosition({windowSize.x / 2.f - 70.f, windowSize.y / 2.f + 100.f});
        prompt.setFillColor(sf::Color(80, 140, 80));
        window.draw(prompt);
    }

    void handleEvent(const sf::Event& event) override;

private:
    sf::Vector2f windowSize;
    RunState stats;
    float celebrationTimer = 0.f;
};
