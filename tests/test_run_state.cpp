#include <catch2/catch_all.hpp>
#include "game/RunState.hpp"

TEST_CASE("RunState initial values", "[runstate]") {
    RunState state;

    REQUIRE(state.playerHealth == 3);
    REQUIRE(state.maxHealth == 3);
    REQUIRE(state.currentFloor == 1);
    REQUIRE(state.roomsVisited == 0);
    REQUIRE(state.enemiesKilled == 0);
    REQUIRE(state.pickupsCollected == 0);
    REQUIRE(state.visitedRooms.empty());
}

TEST_CASE("RunState visitRoom increments counter", "[runstate]") {
    RunState state;

    state.visitRoom(0);
    REQUIRE(state.roomsVisited == 1);
    REQUIRE(state.visitedRooms.count(0) == 1);

    state.visitRoom(1);
    REQUIRE(state.roomsVisited == 2);
    REQUIRE(state.visitedRooms.count(1) == 1);
}

TEST_CASE("RunState visitRoom is idempotent", "[runstate]") {
    RunState state;

    state.visitRoom(0);
    state.visitRoom(0);
    state.visitRoom(0);

    REQUIRE(state.roomsVisited == 1);
    REQUIRE(state.visitedRooms.size() == 1);
}

TEST_CASE("RunState advanceFloor", "[runstate]") {
    RunState state;
    state.visitRoom(0);
    state.visitRoom(1);
    state.visitRoom(2);

    REQUIRE(state.currentFloor == 1);
    REQUIRE(state.visitedRooms.size() == 3);

    state.advanceFloor();

    REQUIRE(state.currentFloor == 2);
    REQUIRE(state.visitedRooms.empty());
}

TEST_CASE("RunState reset clears all state", "[runstate]") {
    RunState state;
    state.playerHealth = 1;
    state.currentFloor = 3;
    state.enemiesKilled = 10;
    state.pickupsCollected = 5;
    state.visitRoom(0);
    state.visitRoom(1);

    state.reset();

    REQUIRE(state.playerHealth == 3);
    REQUIRE(state.maxHealth == 3);
    REQUIRE(state.currentFloor == 1);
    REQUIRE(state.roomsVisited == 0);
    REQUIRE(state.enemiesKilled == 0);
    REQUIRE(state.pickupsCollected == 0);
    REQUIRE(state.visitedRooms.empty());
}
