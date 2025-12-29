#include <catch2/catch_all.hpp>
#include "game/Room.hpp"
#include "ecs/EntityManager.hpp"
#include "core/EventBus.hpp"
#include <algorithm>

TEST_CASE("Room has four doors", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});

    const auto& doors = room.getDoors();
    REQUIRE(doors.size() == 4);

    // Check all directions present
    bool hasNorth = false, hasSouth = false, hasEast = false, hasWest = false;
    for (const auto& door : doors) {
        if (door.direction == Direction::North) hasNorth = true;
        if (door.direction == Direction::South) hasSouth = true;
        if (door.direction == Direction::East) hasEast = true;
        if (door.direction == Direction::West) hasWest = true;
    }

    REQUIRE(hasNorth);
    REQUIRE(hasSouth);
    REQUIRE(hasEast);
    REQUIRE(hasWest);
}

TEST_CASE("Room door connection", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});

    room.connectDoor(Direction::North, 1);
    room.connectDoor(Direction::East, 2);

    const auto& doors = room.getDoors();

    auto northDoor = std::find_if(doors.begin(), doors.end(),
        [](const Door& d) { return d.direction == Direction::North; });

    auto eastDoor = std::find_if(doors.begin(), doors.end(),
        [](const Door& d) { return d.direction == Direction::East; });

    auto southDoor = std::find_if(doors.begin(), doors.end(),
        [](const Door& d) { return d.direction == Direction::South; });

    REQUIRE(northDoor->targetRoomId == 1);
    REQUIRE(eastDoor->targetRoomId == 2);
    REQUIRE(southDoor->targetRoomId == -1);  // Not connected
}

TEST_CASE("Room doors start locked", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});
    room.connectDoor(Direction::North, 1);

    const auto& doors = room.getDoors();
    for (const auto& door : doors) {
        REQUIRE(door.locked == true);
    }
}

TEST_CASE("Room Start type is auto-cleared", "[room]") {
    Room room(0, RoomType::Start, {800.f, 600.f});
    EntityManager manager;
    EventBus::instance().clear();

    room.connectDoor(Direction::North, 1);

    // Simulate entering
    REQUIRE_FALSE(room.isCleared());

    room.update(manager);

    REQUIRE(room.isCleared());
}

TEST_CASE("Room Exit type is auto-cleared", "[room]") {
    Room room(0, RoomType::Exit, {800.f, 600.f});
    EntityManager manager;
    EventBus::instance().clear();

    REQUIRE_FALSE(room.isCleared());

    room.update(manager);

    REQUIRE(room.isCleared());
}

TEST_CASE("Room spawn position from door", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});

    // Coming from north should spawn in south area
    auto fromNorth = room.getSpawnFromDoor(Direction::North);
    REQUIRE(fromNorth.y > 300.f);  // South half

    // Coming from south should spawn in north area
    auto fromSouth = room.getSpawnFromDoor(Direction::South);
    REQUIRE(fromSouth.y < 300.f);  // North half

    // Coming from east should spawn in west area
    auto fromEast = room.getSpawnFromDoor(Direction::East);
    REQUIRE(fromEast.x < 400.f);  // West half

    // Coming from west should spawn in east area
    auto fromWest = room.getSpawnFromDoor(Direction::West);
    REQUIRE(fromWest.x > 400.f);  // East half
}

TEST_CASE("Room Combat clears when no enemies", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});
    room.setEnemyCount(0, 0);  // No enemies spawned
    EntityManager manager;
    EventBus::instance().clear();

    room.connectDoor(Direction::North, 1);

    REQUIRE_FALSE(room.isCleared());

    room.update(manager);

    // With no enemies, room should be cleared
    REQUIRE(room.isCleared());
}

TEST_CASE("Room getId returns correct ID", "[room]") {
    Room room1(0, RoomType::Start, {800.f, 600.f});
    Room room2(5, RoomType::Combat, {800.f, 600.f});
    Room room3(42, RoomType::Exit, {800.f, 600.f});

    REQUIRE(room1.getId() == 0);
    REQUIRE(room2.getId() == 5);
    REQUIRE(room3.getId() == 42);
}

TEST_CASE("Room getType returns correct type", "[room]") {
    Room start(0, RoomType::Start, {800.f, 600.f});
    Room combat(1, RoomType::Combat, {800.f, 600.f});
    Room exit(2, RoomType::Exit, {800.f, 600.f});

    REQUIRE(start.getType() == RoomType::Start);
    REQUIRE(combat.getType() == RoomType::Combat);
    REQUIRE(exit.getType() == RoomType::Exit);
}

TEST_CASE("Room getBounds", "[room]") {
    Room room(0, RoomType::Combat, {800.f, 600.f});

    const auto& bounds = room.getBounds();

    // Bounds should be inset from room size (40px margin)
    REQUIRE(bounds.position.x == Catch::Approx(40.f));
    REQUIRE(bounds.position.y == Catch::Approx(40.f));
    REQUIRE(bounds.size.x == Catch::Approx(720.f));  // 800 - 80
    REQUIRE(bounds.size.y == Catch::Approx(520.f));  // 600 - 80
}
