#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../core/AssetManager.hpp"
#include "../game/RunState.hpp"
#include "../ui/MenuButton.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

class PlayingState;

class VictoryState : public GameState {
public:
    VictoryState(sf::Vector2f windowSize, const RunState& runState);

    void enter() override;
    void exit() override {}

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;

private:
    void setupUI();

    sf::Vector2f windowSize;
    RunState stats;
    float celebrationTimer = 0.f;

    std::optional<sf::Text> title;
    std::optional<sf::Text> floorsText;
    std::optional<sf::Text> enemiesText;
    std::optional<sf::Text> roomsText;
    std::optional<sf::Text> pickupsText;

    std::vector<MenuButton> buttons;
};
