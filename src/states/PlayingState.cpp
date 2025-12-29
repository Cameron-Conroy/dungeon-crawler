#include "PlayingState.hpp"
#include "GameOverState.hpp"
#include "VictoryState.hpp"
#include "PausedState.hpp"
#include "../core/StateManager.hpp"

PlayingState::PlayingState(sf::Vector2f windowSize)
    : windowSize(windowSize) {}

void PlayingState::enter() {
    runState.reset();
    EventBus::instance().clear();
    setupEventHandlers();

    floor = std::make_unique<Floor>(runState.currentFloor, windowSize);
    enterRoom();
}

void PlayingState::exit() {
    entities.clear();
    EventBus::instance().clear();
}

void PlayingState::setupEventHandlers() {
    EventBus::instance().subscribe<PlayerDiedEvent>([this](const PlayerDiedEvent&) {
        manager->push(std::make_unique<GameOverState>(windowSize, runState));
    });

    EventBus::instance().subscribe<EnemyDiedEvent>([this](const EnemyDiedEvent& e) {
        runState.enemiesKilled++;

        // 30% chance to spawn health pickup
        if (std::rand() % 100 < 30) {
            EntityFactory::createHealthPickup(entities, {e.x, e.y});
        }
    });

    EventBus::instance().subscribe<PickupCollectedEvent>([this](const PickupCollectedEvent& e) {
        runState.pickupsCollected++;

        // Sync run state health with player
        Entity* player = getPlayer();
        if (player) {
            auto* health = player->getComponent<HealthComponent>();
            if (health) {
                runState.playerHealth = health->current;
            }
        }
    });

    EventBus::instance().subscribe<PlayerDamagedEvent>([this](const PlayerDamagedEvent& e) {
        Entity* player = getPlayer();
        if (player) {
            auto* health = player->getComponent<HealthComponent>();
            if (health) {
                runState.playerHealth = health->current;
            }
        }
    });
}

void PlayingState::enterRoom() {
    Room* room = floor->getCurrentRoom();
    if (!room) return;

    entities.clear();

    // Create player
    sf::Vector2f spawnPos = floor->getPlayerSpawnPosition();
    auto& player = EntityFactory::createPlayer(entities, spawnPos, room->getBounds());

    // Restore player health from run state
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->current = runState.playerHealth;
        health->max = runState.maxHealth;
    }

    // Spawn enemies for combat rooms
    if (room->getType() == RoomType::Combat && !room->isCleared()) {
        int minEnemies = 2 + runState.currentFloor / 2;
        int maxEnemies = 4 + runState.currentFloor / 2;
        int count = minEnemies + (std::rand() % (maxEnemies - minEnemies + 1));

        const auto& bounds = room->getBounds();
        for (int i = 0; i < count; ++i) {
            float x = bounds.position.x + 50.f + (std::rand() % static_cast<int>(bounds.size.x - 100.f));
            float y = bounds.position.y + 50.f + (std::rand() % static_cast<int>(bounds.size.y - 100.f));

            EntityFactory::EnemyType type = (std::rand() % 3 == 0)
                ? EntityFactory::EnemyType::Bat
                : EntityFactory::EnemyType::Slime;

            EntityFactory::createEnemy(entities, type, {x, y}, bounds);
        }
    }

    runState.visitRoom(room->getId());
}

void PlayingState::update(float dt) {
    if (transitioning) {
        transitionTimer -= dt;
        if (transitionTimer <= 0.f) {
            transitioning = false;
            floor->transitionToRoom(pendingRoomId, pendingDirection);
            enterRoom();
        }
        return;
    }

    // Get player position for AI
    sf::Vector2f playerPos{0.f, 0.f};
    Entity* player = getPlayer();
    if (player) {
        playerPos = player->position;
    }

    // Update systems
    playerControlSystem.update(entities, dt);
    aiSystem.update(entities, dt, playerPos);
    physicsSystem.update(entities, dt);
    collisionSystem.update(entities);
    pickupSystem.update(entities);
    healthSystem.update(entities, dt);

    entities.cleanup();

    // Update room state
    Room* room = floor->getCurrentRoom();
    if (room) {
        room->update(entities);
        checkRoomTransitions();
    }

    // Sync player health to run state
    if (player) {
        auto* health = player->getComponent<HealthComponent>();
        if (health) {
            runState.playerHealth = health->current;
        }
    }
}

void PlayingState::checkRoomTransitions() {
    Entity* player = getPlayer();
    if (!player) return;

    Room* room = floor->getCurrentRoom();
    if (!room) return;

    // Check door transitions
    Door* door = room->checkDoorCollision(player->position, {32.f, 32.f});
    if (door && door->targetRoomId >= 0) {
        transitionToRoom(door->targetRoomId, door->direction);
        return;
    }

    // Check floor exit
    if (room->checkExitCollision(player->position)) {
        if (runState.currentFloor >= MAX_FLOOR) {
            // Victory!
            manager->push(std::make_unique<VictoryState>(windowSize, runState));
        } else {
            // Next floor
            runState.advanceFloor();
            floor = std::make_unique<Floor>(runState.currentFloor, windowSize);
            enterRoom();
        }
    }
}

void PlayingState::transitionToRoom(int targetId, Direction fromDir) {
    // Calculate opposite direction for spawn
    Direction opposite;
    switch (fromDir) {
        case Direction::North: opposite = Direction::South; break;
        case Direction::South: opposite = Direction::North; break;
        case Direction::East:  opposite = Direction::West; break;
        case Direction::West:  opposite = Direction::East; break;
    }

    transitioning = true;
    transitionTimer = TRANSITION_DURATION;
    pendingRoomId = targetId;
    pendingDirection = opposite;
}

void PlayingState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 50));

    // Draw room
    Room* room = floor->getCurrentRoom();
    if (room) {
        room->render(window);
    }

    // Draw entities
    renderSystem.render(entities, window);

    // Draw UI
    renderUI(window);

    // Draw minimap
    renderMinimap(window);

    // Transition fade
    if (transitioning) {
        float alpha = (1.f - transitionTimer / TRANSITION_DURATION) * 255.f;
        sf::RectangleShape fade(windowSize);
        fade.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
        window.draw(fade);
    }
}

void PlayingState::renderUI(sf::RenderWindow& window) {
    // Health hearts
    for (int i = 0; i < runState.maxHealth; ++i) {
        sf::RectangleShape heart({20.f, 20.f});
        heart.setPosition({10.f + i * 25.f, 10.f});

        if (i < runState.playerHealth) {
            heart.setFillColor(sf::Color::Red);
        } else {
            heart.setFillColor(sf::Color(80, 40, 40));
        }
        window.draw(heart);
    }

    // Floor indicator
    sf::RectangleShape floorBg({80.f, 30.f});
    floorBg.setPosition({windowSize.x - 90.f, 10.f});
    floorBg.setFillColor(sf::Color(40, 40, 60));
    window.draw(floorBg);

    // Floor number as simple bars
    for (int i = 0; i < runState.currentFloor; ++i) {
        sf::RectangleShape bar({15.f, 20.f});
        bar.setPosition({windowSize.x - 85.f + i * 20.f, 15.f});
        bar.setFillColor(sf::Color(200, 200, 100));
        window.draw(bar);
    }
}

void PlayingState::renderMinimap(sf::RenderWindow& window) {
    float mapX = windowSize.x - 120.f;
    float mapY = 50.f;
    float roomW = 15.f;
    float roomH = 12.f;
    float gap = 2.f;

    const auto& positions = floor->getRoomPositions();
    int currentId = floor->getCurrentRoomId();

    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (const auto& [id, pos] : positions) {
        minX = std::min(minX, pos.x);
        maxX = std::max(maxX, pos.x);
        minY = std::min(minY, pos.y);
        maxY = std::max(maxY, pos.y);
    }

    // Draw rooms
    for (const auto& [id, pos] : positions) {
        float x = mapX + (pos.x - minX) * (roomW + gap);
        float y = mapY + (pos.y - minY) * (roomH + gap);

        sf::RectangleShape roomShape({roomW, roomH});
        roomShape.setPosition({x, y});

        if (id == currentId) {
            roomShape.setFillColor(sf::Color(100, 200, 100));
        } else if (runState.visitedRooms.count(id)) {
            roomShape.setFillColor(sf::Color(100, 100, 140));
        } else {
            roomShape.setFillColor(sf::Color(60, 60, 80));
        }

        window.draw(roomShape);
    }
}

void PlayingState::handleEvent(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            manager->push(std::make_unique<PausedState>(windowSize));
        }
    }
}

Entity* PlayingState::getPlayer() {
    Entity* result = nullptr;
    entities.forEachWith<PlayerControlComponent>([&result](Entity& e) {
        result = &e;
    });
    return result;
}
