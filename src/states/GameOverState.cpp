#include "GameOverState.hpp"
#include "PlayingState.hpp"
#include "MainMenuState.hpp"
#include "../core/StateManager.hpp"

GameOverState::GameOverState(sf::Vector2f windowSize, const RunState& runState)
    : windowSize(windowSize), stats(runState) {}

void GameOverState::enter() {
    setupUI();
}

void GameOverState::setupUI() {
    buttons.clear();

    if (AssetManager::instance().hasFont("pixel")) {
        auto& font = AssetManager::instance().getFont("pixel");

        // Title
        title.emplace(font, "GAME OVER", 24);
        title->setFillColor(sf::Color(200, 80, 80));
        sf::FloatRect titleBounds = title->getLocalBounds();
        title->setPosition({windowSize.x / 2.f - titleBounds.size.x / 2.f, windowSize.y / 2.f - 100.f});

        // Stats
        floorText.emplace(font, "Floor: " + std::to_string(stats.currentFloor), 12);
        floorText->setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect floorBounds = floorText->getLocalBounds();
        floorText->setPosition({windowSize.x / 2.f - floorBounds.size.x / 2.f, windowSize.y / 2.f - 40.f});

        enemiesText.emplace(font, "Enemies: " + std::to_string(stats.enemiesKilled), 12);
        enemiesText->setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect enemiesBounds = enemiesText->getLocalBounds();
        enemiesText->setPosition({windowSize.x / 2.f - enemiesBounds.size.x / 2.f, windowSize.y / 2.f - 15.f});

        roomsText.emplace(font, "Rooms: " + std::to_string(stats.roomsVisited), 12);
        roomsText->setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect roomsBounds = roomsText->getLocalBounds();
        roomsText->setPosition({windowSize.x / 2.f - roomsBounds.size.x / 2.f, windowSize.y / 2.f + 10.f});
    }

    // Create buttons
    buttons.emplace_back("RETRY", 16);
    buttons.emplace_back("QUIT", 16);

    // Position buttons centered
    float buttonY = windowSize.y / 2.f + 50.f;
    float buttonSpacing = 40.f;

    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::FloatRect bounds = buttons[i].getBounds();
        float x = windowSize.x / 2.f - bounds.size.x / 2.f;
        buttons[i].setPosition(x, buttonY + i * buttonSpacing);
    }
}

void GameOverState::render(sf::RenderWindow& window) {
    // Dark red overlay
    sf::RectangleShape overlay(windowSize);
    overlay.setFillColor(sf::Color(20, 0, 0, 200));
    window.draw(overlay);

    // Get mouse position for hover detection
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Update buttons with mouse position
    for (auto& button : buttons) {
        button.update(mousePos, ASSUMED_DT);
    }

    // Box background
    sf::RectangleShape box({320.f, 260.f});
    box.setPosition({windowSize.x / 2.f - 160.f, windowSize.y / 2.f - 120.f});
    box.setFillColor(sf::Color(40, 30, 30));
    box.setOutlineColor(sf::Color(150, 50, 50));
    box.setOutlineThickness(4.f);
    window.draw(box);

    // Draw title
    if (title) window.draw(*title);

    // Draw stats
    if (floorText) window.draw(*floorText);
    if (enemiesText) window.draw(*enemiesText);
    if (roomsText) window.draw(*roomsText);

    // Draw buttons
    for (auto& button : buttons) {
        button.render(window);
    }
}

void GameOverState::handleEvent(const sf::Event& event) {
    // Keyboard input
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R ||
            keyPressed->code == sf::Keyboard::Key::Enter) {
            manager->reset(std::make_unique<PlayingState>(windowSize));
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape ||
            keyPressed->code == sf::Keyboard::Key::Q) {
            manager->reset(std::make_unique<MainMenuState>(windowSize));
        }
    }

    // Mouse input
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            if (buttons.size() > 0 && buttons[0].isHovered()) {
                manager->reset(std::make_unique<PlayingState>(windowSize));
            }
            if (buttons.size() > 1 && buttons[1].isHovered()) {
                manager->reset(std::make_unique<MainMenuState>(windowSize));
            }
        }
    }
}
