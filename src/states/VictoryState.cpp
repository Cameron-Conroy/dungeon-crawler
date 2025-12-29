#include "VictoryState.hpp"
#include "PlayingState.hpp"
#include "../core/StateManager.hpp"

void VictoryState::handleEvent(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R ||
            keyPressed->code == sf::Keyboard::Key::Enter) {
            manager->pop();  // Remove victory
            manager->pop();  // Remove playing
            manager->push(std::make_unique<PlayingState>(windowSize));
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape ||
            keyPressed->code == sf::Keyboard::Key::Q) {
            manager->pop();  // Remove victory
            manager->pop();  // Remove playing
        }
    }
}
