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

class GameOverState : public GameState {
public:
    GameOverState(sf::Vector2f windowSize, const RunState& runState);

    void enter() override;
    void exit() override {}

    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;

private:
    void setupUI();

    // Assumed delta time for button animations when actual dt unavailable in render
    static constexpr float ASSUMED_DT = 1.f / 60.f;

    sf::Vector2f windowSize;
    RunState stats;

    std::optional<sf::Text> title;
    std::optional<sf::Text> floorText;
    std::optional<sf::Text> enemiesText;
    std::optional<sf::Text> roomsText;

    std::vector<MenuButton> buttons;
};
