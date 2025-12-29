#pragma once

#include "../core/GameState.hpp"
#include "../core/StateManager.hpp"
#include "../core/EventBus.hpp"
#include "../ecs/EntityManager.hpp"
#include "../ecs/Systems.hpp"
#include "../ecs/EntityFactory.hpp"
#include "../game/Floor.hpp"
#include "../game/RunState.hpp"
#include <memory>

class GameOverState;
class VictoryState;
class PausedState;

class PlayingState : public GameState {
public:
    PlayingState(sf::Vector2f windowSize);

    void enter() override;
    void exit() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;

private:
    void setupEventHandlers();
    void enterRoom();
    void transitionToRoom(int targetId, Direction fromDir);
    void checkRoomTransitions();
    void renderUI(sf::RenderWindow& window);
    void renderMinimap(sf::RenderWindow& window);
    Entity* getPlayer();

    sf::Vector2f windowSize;

    EntityManager entities;
    PhysicsSystem physicsSystem;
    AISystem aiSystem;
    PlayerControlSystem playerControlSystem;
    CollisionSystem collisionSystem;
    PickupSystem pickupSystem;
    RenderSystem renderSystem;
    HealthSystem healthSystem;

    std::unique_ptr<Floor> floor;
    RunState runState;

    bool transitioning = false;
    float transitionTimer = 0.f;
    int pendingRoomId = -1;
    Direction pendingDirection = Direction::South;

    static constexpr float TRANSITION_DURATION = 0.3f;
    static constexpr int MAX_FLOOR = 3;
};
