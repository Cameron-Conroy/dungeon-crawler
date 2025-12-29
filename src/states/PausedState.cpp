#include "PausedState.hpp"
#include "PlayingState.hpp"
#include "MainMenuState.hpp"
#include "../core/StateManager.hpp"

PausedState::PausedState(sf::Vector2f windowSize)
    : windowSize(windowSize) {}

void PausedState::enter() {
    setupUI();
}

void PausedState::setupUI() {
    buttons.clear();

    // Setup title
    if (AssetManager::instance().hasFont("pixel")) {
        auto& font = AssetManager::instance().getFont("pixel");

        title.emplace(font, "PAUSED", 24);
        title->setFillColor(sf::Color::White);
        sf::FloatRect bounds = title->getLocalBounds();
        title->setPosition({windowSize.x / 2.f - bounds.size.x / 2.f, windowSize.y / 2.f - 80.f});
    }

    // Create buttons
    buttons.emplace_back("RESUME", 16);
    buttons.emplace_back("RESTART", 16);
    buttons.emplace_back("QUIT", 16);

    // Position buttons centered
    float buttonY = windowSize.y / 2.f - 20.f;
    float buttonSpacing = 40.f;

    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::FloatRect bounds = buttons[i].getBounds();
        float x = windowSize.x / 2.f - bounds.size.x / 2.f;
        buttons[i].setPosition(x, buttonY + i * buttonSpacing);
    }
}

void PausedState::render(sf::RenderWindow& window) {
    // Semi-transparent overlay
    sf::RectangleShape overlay(windowSize);
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // Get mouse position for hover detection
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Update buttons with mouse position
    for (auto& button : buttons) {
        button.update(mousePos, ASSUMED_DT);
    }

    // Pause box background
    sf::RectangleShape box({280.f, 200.f});
    box.setPosition({windowSize.x / 2.f - 140.f, windowSize.y / 2.f - 100.f});
    box.setFillColor(sf::Color(40, 40, 55));
    box.setOutlineColor(sf::Color(100, 100, 140));
    box.setOutlineThickness(3.f);
    window.draw(box);

    // Draw title
    if (title) window.draw(*title);

    // Draw buttons
    for (auto& button : buttons) {
        button.render(window);
    }
}

void PausedState::handleEvent(const sf::Event& event) {
    // Keyboard input
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape ||
            keyPressed->code == sf::Keyboard::Key::P) {
            manager->pop();  // Resume
        }
    }

    // Mouse input
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            if (buttons.size() > 0 && buttons[0].isHovered()) {
                manager->pop();  // Resume
            }
            if (buttons.size() > 1 && buttons[1].isHovered()) {
                // Restart
                manager->reset(std::make_unique<PlayingState>(windowSize));
            }
            if (buttons.size() > 2 && buttons[2].isHovered()) {
                // Quit to main menu
                manager->reset(std::make_unique<MainMenuState>(windowSize));
            }
        }
    }
}
