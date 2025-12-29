#include "VictoryState.hpp"
#include "PlayingState.hpp"
#include "MainMenuState.hpp"
#include "../core/StateManager.hpp"

VictoryState::VictoryState(sf::Vector2f windowSize, const RunState& runState)
    : windowSize(windowSize), stats(runState) {}

void VictoryState::enter() {
    setupUI();
}

void VictoryState::update(float dt) {
    celebrationTimer += dt;
}

void VictoryState::setupUI() {
    buttons.clear();

    if (AssetManager::instance().hasFont("pixel")) {
        auto& font = AssetManager::instance().getFont("pixel");

        // Title
        title.emplace(font, "VICTORY!", 28);
        title->setFillColor(sf::Color(255, 220, 100));
        sf::FloatRect titleBounds = title->getLocalBounds();
        title->setPosition({windowSize.x / 2.f - titleBounds.size.x / 2.f, windowSize.y / 2.f - 120.f});

        // Stats
        floorsText.emplace(font, "Floors: " + std::to_string(stats.currentFloor), 12);
        floorsText->setFillColor(sf::Color(180, 180, 150));
        sf::FloatRect floorsBounds = floorsText->getLocalBounds();
        floorsText->setPosition({windowSize.x / 2.f - floorsBounds.size.x / 2.f, windowSize.y / 2.f - 60.f});

        enemiesText.emplace(font, "Enemies: " + std::to_string(stats.enemiesKilled), 12);
        enemiesText->setFillColor(sf::Color(180, 180, 150));
        sf::FloatRect enemiesBounds = enemiesText->getLocalBounds();
        enemiesText->setPosition({windowSize.x / 2.f - enemiesBounds.size.x / 2.f, windowSize.y / 2.f - 35.f});

        roomsText.emplace(font, "Rooms: " + std::to_string(stats.roomsVisited), 12);
        roomsText->setFillColor(sf::Color(180, 180, 150));
        sf::FloatRect roomsBounds = roomsText->getLocalBounds();
        roomsText->setPosition({windowSize.x / 2.f - roomsBounds.size.x / 2.f, windowSize.y / 2.f - 10.f});

        pickupsText.emplace(font, "Pickups: " + std::to_string(stats.pickupsCollected), 12);
        pickupsText->setFillColor(sf::Color(180, 180, 150));
        sf::FloatRect pickupsBounds = pickupsText->getLocalBounds();
        pickupsText->setPosition({windowSize.x / 2.f - pickupsBounds.size.x / 2.f, windowSize.y / 2.f + 15.f});
    }

    // Create buttons
    buttons.emplace_back("PLAY AGAIN", 16);
    buttons.emplace_back("QUIT", 16);

    // Position buttons centered
    float buttonY = windowSize.y / 2.f + 60.f;
    float buttonSpacing = 40.f;

    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::FloatRect bounds = buttons[i].getBounds();
        float x = windowSize.x / 2.f - bounds.size.x / 2.f;
        buttons[i].setPosition(x, buttonY + i * buttonSpacing);
    }
}

void VictoryState::render(sf::RenderWindow& window) {
    // Golden pulsing overlay
    float pulse = 0.5f + 0.3f * std::sin(celebrationTimer * 4.f);
    sf::RectangleShape overlay(windowSize);
    overlay.setFillColor(sf::Color(50, 50, 20, static_cast<std::uint8_t>(180 * pulse)));
    window.draw(overlay);

    // Get mouse position for hover detection
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Update buttons with mouse position
    for (auto& button : buttons) {
        button.update(mousePos, ASSUMED_DT);
    }

    // Box background
    sf::RectangleShape box({340.f, 280.f});
    box.setPosition({windowSize.x / 2.f - 170.f, windowSize.y / 2.f - 140.f});
    box.setFillColor(sf::Color(40, 45, 30));
    box.setOutlineColor(sf::Color(200, 200, 100));
    box.setOutlineThickness(4.f);
    window.draw(box);

    // Draw title
    if (title) window.draw(*title);

    // Draw stats
    if (floorsText) window.draw(*floorsText);
    if (enemiesText) window.draw(*enemiesText);
    if (roomsText) window.draw(*roomsText);
    if (pickupsText) window.draw(*pickupsText);

    // Draw buttons
    for (auto& button : buttons) {
        button.render(window);
    }
}

void VictoryState::handleEvent(const sf::Event& event) {
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
