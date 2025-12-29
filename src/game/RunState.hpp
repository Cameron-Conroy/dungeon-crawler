#pragma once

#include <set>

struct RunState {
    int playerHealth = 3;
    int maxHealth = 3;
    int currentFloor = 1;
    int roomsVisited = 0;
    int enemiesKilled = 0;
    int pickupsCollected = 0;
    std::set<int> visitedRooms;

    void reset() {
        playerHealth = 3;
        maxHealth = 3;
        currentFloor = 1;
        roomsVisited = 0;
        enemiesKilled = 0;
        pickupsCollected = 0;
        visitedRooms.clear();
    }

    void visitRoom(int roomId) {
        if (visitedRooms.find(roomId) == visitedRooms.end()) {
            visitedRooms.insert(roomId);
            roomsVisited++;
        }
    }

    void advanceFloor() {
        currentFloor++;
        visitedRooms.clear();
    }
};
