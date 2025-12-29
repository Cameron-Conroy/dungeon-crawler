#include "MainMenuState.hpp"
#include "PlayingState.hpp"
#include "../core/StateManager.hpp"

void MainMenuState::handleEvent(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space) {
            manager->swap(std::make_unique<PlayingState>(windowSize));
        }
    }
}
