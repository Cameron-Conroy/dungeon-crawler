#pragma once

#include "GameState.hpp"
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

class StateManager {
public:
    void push(std::unique_ptr<GameState> state) {
        // Note: Don't call exit() on current state when pushing
        // The current state is paused, not exited (it stays on the stack)
        state->setManager(this);
        states.push_back(std::move(state));
        states.back()->enter();
    }

    void pop() {
        if (!states.empty()) {
            states.back()->exit();
            pendingPop = true;
        }
    }

    void swap(std::unique_ptr<GameState> state) {
        if (!states.empty()) {
            states.back()->exit();
            states.pop_back();
        }
        state->setManager(this);
        states.push_back(std::move(state));
        states.back()->enter();
    }

    void reset(std::unique_ptr<GameState> state) {
        while (!states.empty()) {
            states.back()->exit();
            states.pop_back();
        }
        pendingPop = false;
        state->setManager(this);
        states.push_back(std::move(state));
        states.back()->enter();
    }

    void update(float dt) {
        processPending();
        if (!states.empty()) {
            states.back()->update(dt);
        }
    }

    void render(sf::RenderWindow& window) {
        if (!states.empty()) {
            states.back()->render(window);
        }
    }

    void handleEvent(const sf::Event& event) {
        if (!states.empty()) {
            states.back()->handleEvent(event);
        }
    }

    bool empty() const {
        return states.empty();
    }

    GameState* current() {
        return states.empty() ? nullptr : states.back().get();
    }

private:
    void processPending() {
        if (pendingPop && !states.empty()) {
            states.pop_back();
            pendingPop = false;
        }
    }

    std::vector<std::unique_ptr<GameState>> states;
    bool pendingPop = false;
};
