#include "MainMenuState.hpp"
#include "PlayingState.hpp"
#include "../core/StateManager.hpp"

MainMenuState::MainMenuState(sf::Vector2f windowSize)
    : windowSize(windowSize) {}

void MainMenuState::enter() {
    setupUI();
}

void MainMenuState::setupUI() {
    buttons.clear();

    // Setup title text
    if (AssetManager::instance().hasFont("pixel")) {
        auto& font = AssetManager::instance().getFont("pixel");

        titleLine1.emplace(font, "DUNGEON", 32);
        titleLine1->setFillColor(sf::Color(100, 180, 100));
        sf::FloatRect bounds1 = titleLine1->getLocalBounds();
        titleLine1->setPosition({windowSize.x / 2.f - bounds1.size.x / 2.f, 120.f});

        titleLine2.emplace(font, "CRAWLER", 32);
        titleLine2->setFillColor(sf::Color(100, 180, 100));
        sf::FloatRect bounds2 = titleLine2->getLocalBounds();
        titleLine2->setPosition({windowSize.x / 2.f - bounds2.size.x / 2.f, 170.f});
    }

    // Create buttons
    buttons.emplace_back("START GAME", 16);
    buttons.emplace_back("QUIT", 16);

    // Position buttons centered
    float buttonY = 300.f;
    float buttonSpacing = 50.f;

    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::FloatRect bounds = buttons[i].getBounds();
        float x = windowSize.x / 2.f - bounds.size.x / 2.f;
        buttons[i].setPosition(x, buttonY + i * buttonSpacing);
    }
}

void MainMenuState::update(float dt) {
    pulseTimer += dt;
}

void MainMenuState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Get mouse position for hover detection
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Update buttons with mouse position
    for (auto& button : buttons) {
        button.update(mousePos, 0.016f);
    }

    // Draw title
    if (titleLine1) window.draw(*titleLine1);
    if (titleLine2) window.draw(*titleLine2);

    // Draw buttons
    for (auto& button : buttons) {
        button.render(window);
    }
}

void MainMenuState::handleEvent(const sf::Event& event) {
    // Keyboard input
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space) {
            manager->swap(std::make_unique<PlayingState>(windowSize));
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            manager->pop();  // Quit
        }
    }

    // Mouse input
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            // Check which button was clicked
            if (buttons.size() > 0 && buttons[0].isHovered()) {
                manager->swap(std::make_unique<PlayingState>(windowSize));
            }
            if (buttons.size() > 1 && buttons[1].isHovered()) {
                manager->pop();  // Quit
            }
        }
    }
}
