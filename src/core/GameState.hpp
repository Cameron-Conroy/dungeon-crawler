#pragma once

#include <SFML/Graphics.hpp>

class StateManager;

class GameState {
public:
    virtual ~GameState() = default;

    virtual void enter() {}
    virtual void exit() {}
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;

    void setManager(StateManager* mgr) { manager = mgr; }

protected:
    StateManager* manager = nullptr;
};
