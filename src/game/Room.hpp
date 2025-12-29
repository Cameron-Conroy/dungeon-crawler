#pragma once

#include "../ecs/EntityManager.hpp"
#include "../ecs/EntityFactory.hpp"
#include "../core/EventBus.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

enum class Direction { North, South, East, West };

struct Door {
    Direction direction;
    int targetRoomId = -1;
    bool locked = true;
    sf::FloatRect bounds;

    Door(Direction dir, sf::FloatRect rect) : direction(dir), bounds(rect) {}
};

enum class RoomType { Start, Combat, Exit };

class Room {
public:
    Room(int id, RoomType type, sf::Vector2f size)
        : id(id), type(type), size(size), bounds({40.f, 40.f}, {size.x - 80.f, size.y - 80.f}) {
        setupDoors();
    }

    // Note: Room no longer manages entity spawning or event subscriptions.
    // PlayingState handles player/enemy creation and event handling directly.
    // This avoids subscription accumulation (issue #2) and code duplication (issue #4).

    void update(EntityManager& entities) {
        if (!cleared && type == RoomType::Combat) {
            // Check if all enemies are dead
            size_t enemyCount = entities.countWith<EnemyTag>();
            if (enemyCount == 0) {
                cleared = true;
                unlockDoors();
                EventBus::instance().emit<RoomClearedEvent>(id);
            }
        }

        // Start rooms and exit rooms are always "cleared"
        if (type == RoomType::Start || type == RoomType::Exit) {
            if (!cleared) {
                cleared = true;
                unlockDoors();
            }
        }
    }

    void render(sf::RenderWindow& window) {
        // Draw room background
        sf::RectangleShape roomShape(bounds.size);
        roomShape.setPosition(bounds.position);
        roomShape.setFillColor(sf::Color(60, 60, 70));
        roomShape.setOutlineColor(sf::Color(100, 100, 120));
        roomShape.setOutlineThickness(4.f);
        window.draw(roomShape);

        // Draw doors
        for (const auto& door : doors) {
            if (door.targetRoomId < 0) continue;  // No connection

            sf::RectangleShape doorShape({door.bounds.size.x, door.bounds.size.y});
            doorShape.setPosition(door.bounds.position);

            if (door.locked) {
                doorShape.setFillColor(sf::Color(80, 40, 40));
            } else {
                doorShape.setFillColor(sf::Color(100, 150, 100));
            }
            window.draw(doorShape);
        }

        // Draw exit indicator
        if (type == RoomType::Exit && cleared) {
            sf::RectangleShape exitShape({60.f, 60.f});
            exitShape.setPosition({size.x / 2.f - 30.f, size.y / 2.f - 30.f});
            exitShape.setFillColor(sf::Color(200, 200, 100));
            window.draw(exitShape);
        }
    }

    Door* checkDoorCollision(sf::Vector2f playerPos, sf::Vector2f playerSize) {
        sf::FloatRect playerBounds({playerPos.x - playerSize.x / 2.f, playerPos.y - playerSize.y / 2.f}, playerSize);

        for (auto& door : doors) {
            if (door.targetRoomId >= 0 && !door.locked && playerBounds.findIntersection(door.bounds)) {
                return &door;
            }
        }
        return nullptr;
    }

    bool checkExitCollision(sf::Vector2f playerPos) {
        if (type != RoomType::Exit || !cleared) return false;

        sf::FloatRect exitBounds({size.x / 2.f - 30.f, size.y / 2.f - 30.f}, {60.f, 60.f});
        sf::FloatRect playerBounds({playerPos.x - 16.f, playerPos.y - 16.f}, {32.f, 32.f});
        return exitBounds.findIntersection(playerBounds).has_value();
    }

    void connectDoor(Direction dir, int targetId) {
        for (auto& door : doors) {
            if (door.direction == dir) {
                door.targetRoomId = targetId;
                return;
            }
        }
    }

    sf::Vector2f getSpawnFromDoor(Direction fromDir) const {
        // Spawn player opposite to the door they came from
        switch (fromDir) {
            case Direction::North: return {size.x / 2.f, bounds.position.y + bounds.size.y - 50.f};
            case Direction::South: return {size.x / 2.f, bounds.position.y + 50.f};
            case Direction::East:  return {bounds.position.x + 50.f, size.y / 2.f};
            case Direction::West:  return {bounds.position.x + bounds.size.x - 50.f, size.y / 2.f};
        }
        return {size.x / 2.f, size.y / 2.f};
    }

    int getId() const { return id; }
    RoomType getType() const { return type; }
    bool isCleared() const { return cleared; }
    const sf::FloatRect& getBounds() const { return bounds; }
    const std::vector<Door>& getDoors() const { return doors; }

private:
    void setupDoors() {
        float doorWidth = 60.f;
        float doorDepth = 40.f;  // Extended to reach into playable area

        // North door - extends down into playable area
        doors.emplace_back(Direction::North,
            sf::FloatRect({size.x / 2.f - doorWidth / 2.f, 20.f}, {doorWidth, doorDepth + 20.f}));

        // South door - extends up into playable area
        doors.emplace_back(Direction::South,
            sf::FloatRect({size.x / 2.f - doorWidth / 2.f, size.y - 60.f}, {doorWidth, doorDepth + 20.f}));

        // East door - extends left into playable area
        doors.emplace_back(Direction::East,
            sf::FloatRect({size.x - 60.f, size.y / 2.f - doorWidth / 2.f}, {doorDepth + 20.f, doorWidth}));

        // West door - extends right into playable area
        doors.emplace_back(Direction::West,
            sf::FloatRect({20.f, size.y / 2.f - doorWidth / 2.f}, {doorDepth + 20.f, doorWidth}));
    }

    void unlockDoors() {
        for (auto& door : doors) {
            if (door.targetRoomId >= 0) {
                door.locked = false;
            }
        }
    }

    int id;
    RoomType type;
    sf::Vector2f size;
    sf::FloatRect bounds;
    std::vector<Door> doors;
    bool cleared = false;
};
