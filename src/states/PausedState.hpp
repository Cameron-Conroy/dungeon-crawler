#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../core/AssetManager.hpp"
#include "../ui/MenuButton.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

class PlayingState;

class PausedState : public GameState {
public:
    explicit PausedState(sf::Vector2f windowSize);

    void enter() override;
    void exit() override {}

    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;

private:
    void setupUI();

    sf::Vector2f windowSize;
    std::optional<sf::Text> title;
    std::vector<MenuButton> buttons;
};
