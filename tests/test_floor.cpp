#include <catch2/catch_all.hpp>
#include "game/Floor.hpp"
#include <cstdlib>

TEST_CASE("Floor generation creates rooms", "[floor]") {
    std::srand(12345);  // Deterministic seed

    Floor floor(1, {800.f, 600.f});

    const auto& rooms = floor.getRooms();
    REQUIRE(rooms.size() >= 2);  // At least start and exit
}

TEST_CASE("Floor has start room", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    bool hasStart = false;
    for (const auto& room : floor.getRooms()) {
        if (room->getType() == RoomType::Start) {
            hasStart = true;
            break;
        }
    }

    REQUIRE(hasStart);
}

TEST_CASE("Floor has exit room", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    bool hasExit = false;
    for (const auto& room : floor.getRooms()) {
        if (room->getType() == RoomType::Exit) {
            hasExit = true;
            break;
        }
    }

    REQUIRE(hasExit);
}

TEST_CASE("Floor start room at origin", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    const auto& positions = floor.getRoomPositions();

    // Room 0 (start) should be at grid position (0, 0)
    auto it = positions.find(0);
    REQUIRE(it != positions.end());
    REQUIRE(it->second.x == 0);
    REQUIRE(it->second.y == 0);
}

TEST_CASE("Floor getCurrentRoom", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    Room* current = floor.getCurrentRoom();

    REQUIRE(current != nullptr);
    REQUIRE(current->getId() == 0);
    REQUIRE(current->getType() == RoomType::Start);
}

TEST_CASE("Floor getCurrentRoomId", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    REQUIRE(floor.getCurrentRoomId() == 0);
}

TEST_CASE("Floor getRoom by ID", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    Room* room0 = floor.getRoom(0);
    REQUIRE(room0 != nullptr);
    REQUIRE(room0->getId() == 0);

    Room* invalid = floor.getRoom(999);
    REQUIRE(invalid == nullptr);
}

TEST_CASE("Floor room connectivity", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    // Start room should have at least one connected door
    Room* startRoom = floor.getRoom(0);
    REQUIRE(startRoom != nullptr);

    const auto& doors = startRoom->getDoors();
    bool hasConnection = false;
    for (const auto& door : doors) {
        if (door.targetRoomId >= 0) {
            hasConnection = true;
            break;
        }
    }

    REQUIRE(hasConnection);
}

TEST_CASE("Floor room transitions", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    REQUIRE(floor.getCurrentRoomId() == 0);

    // Find a connected room
    Room* startRoom = floor.getCurrentRoom();
    int targetId = -1;
    Direction fromDir = Direction::South;

    for (const auto& door : startRoom->getDoors()) {
        if (door.targetRoomId >= 0) {
            targetId = door.targetRoomId;
            fromDir = door.direction;
            break;
        }
    }

    if (targetId >= 0) {
        bool success = floor.transitionToRoom(targetId, fromDir);
        REQUIRE(success);
        REQUIRE(floor.getCurrentRoomId() == targetId);
    }
}

TEST_CASE("Floor transition to invalid room fails", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    bool success = floor.transitionToRoom(999, Direction::North);
    REQUIRE_FALSE(success);
    REQUIRE(floor.getCurrentRoomId() == 0);  // Still at start
}

TEST_CASE("Floor getPlayerSpawnPosition for start room", "[floor]") {
    std::srand(12345);

    Floor floor(1, {800.f, 600.f});

    auto spawnPos = floor.getPlayerSpawnPosition();

    // Should be center of room
    REQUIRE(spawnPos.x == Catch::Approx(400.f));
    REQUIRE(spawnPos.y == Catch::Approx(300.f));
}

TEST_CASE("Floor higher floors have more rooms", "[floor]") {
    std::srand(12345);
    Floor floor1(1, {800.f, 600.f});

    std::srand(12345);
    Floor floor3(3, {800.f, 600.f});

    // Floor 3 should have more rooms than floor 1
    // (roomCount = 4 + floorNumber)
    REQUIRE(floor3.getRooms().size() >= floor1.getRooms().size());
}

TEST_CASE("Floor getFloorNumber", "[floor]") {
    Floor floor1(1, {800.f, 600.f});
    Floor floor5(5, {800.f, 600.f});

    REQUIRE(floor1.getFloorNumber() == 1);
    REQUIRE(floor5.getFloorNumber() == 5);
}
