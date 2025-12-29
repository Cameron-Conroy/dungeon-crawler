#include "GameOverState.hpp"
#include "PlayingState.hpp"
#include "MainMenuState.hpp"
#include "../core/StateManager.hpp"

void GameOverState::handleEvent(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R ||
            keyPressed->code == sf::Keyboard::Key::Enter) {
            manager->pop();  // Remove game over
            manager->pop();  // Remove playing
            manager->push(std::make_unique<PlayingState>(windowSize));
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape ||
            keyPressed->code == sf::Keyboard::Key::Q) {
            manager->pop();  // Remove game over
            manager->pop();  // Remove playing
        }
    }
}
