#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../core/AssetManager.hpp"
#include "../ui/MenuButton.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

class PlayingState;

class MainMenuState : public GameState {
public:
    explicit MainMenuState(sf::Vector2f windowSize);

    void enter() override;
    void exit() override {}

    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;

private:
    void setupUI();

    // Assumed delta time for button animations when actual dt unavailable in render
    static constexpr float ASSUMED_DT = 1.f / 60.f;  // ~0.016f at 60 FPS

    sf::Vector2f windowSize;

    std::optional<sf::Text> titleLine1;
    std::optional<sf::Text> titleLine2;

    std::vector<MenuButton> buttons;
    bool mousePressed = false;
};
