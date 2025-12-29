#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include <SFML/Graphics.hpp>

class PausedState : public GameState {
public:
    explicit PausedState(sf::Vector2f windowSize) : windowSize(windowSize) {}

    void enter() override {}
    void exit() override {}

    void update(float dt) override {}

    void render(sf::RenderWindow& window) override {
        // Semi-transparent overlay
        sf::RectangleShape overlay(windowSize);
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        // Pause box
        sf::RectangleShape box({200.f, 150.f});
        box.setPosition({windowSize.x / 2.f - 100.f, windowSize.y / 2.f - 75.f});
        box.setFillColor(sf::Color(50, 50, 70));
        box.setOutlineColor(sf::Color(100, 100, 140));
        box.setOutlineThickness(3.f);
        window.draw(box);

        // "PAUSED" indicator
        sf::RectangleShape pauseText({120.f, 30.f});
        pauseText.setPosition({windowSize.x / 2.f - 60.f, windowSize.y / 2.f - 50.f});
        pauseText.setFillColor(sf::Color(180, 180, 200));
        window.draw(pauseText);

        // Resume hint
        sf::RectangleShape resumeHint({80.f, 20.f});
        resumeHint.setPosition({windowSize.x / 2.f - 40.f, windowSize.y / 2.f});
        resumeHint.setFillColor(sf::Color(100, 150, 100));
        window.draw(resumeHint);

        // Quit hint
        sf::RectangleShape quitHint({60.f, 20.f});
        quitHint.setPosition({windowSize.x / 2.f - 30.f, windowSize.y / 2.f + 35.f});
        quitHint.setFillColor(sf::Color(150, 100, 100));
        window.draw(quitHint);
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape ||
                keyPressed->code == sf::Keyboard::Key::P) {
                manager->pop();  // Resume
            }
            if (keyPressed->code == sf::Keyboard::Key::Q) {
                // Return to main menu
                manager->pop();  // Remove pause
                manager->pop();  // Remove playing
            }
        }
    }

private:
    sf::Vector2f windowSize;
};
