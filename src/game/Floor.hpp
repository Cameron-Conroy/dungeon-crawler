#pragma once

#include "Room.hpp"
#include "../util/Random.hpp"
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

struct GridPos {
    int x, y;

    bool operator==(const GridPos& other) const {
        return x == other.x && y == other.y;
    }
};

class Floor {
public:
    Floor(int floorNumber, sf::Vector2f roomSize)
        : floorNumber(floorNumber), roomSize(roomSize) {
        generate();
    }

    void generate() {
        rooms.clear();
        grid.clear();

        int roomCount = 4 + floorNumber;  // More rooms on later floors
        int nextId = 0;

        // Start room at center
        GridPos startPos{0, 0};
        auto& startRoom = createRoom(nextId++, RoomType::Start, startPos);

        // Generate connected rooms
        std::vector<GridPos> frontier;
        addNeighborsToFrontier(startPos, frontier);

        while (rooms.size() < static_cast<size_t>(roomCount) && !frontier.empty()) {
            // Pick random frontier position
            int idx = util::randomInt(0, static_cast<int>(frontier.size()) - 1);
            GridPos pos = frontier[idx];
            frontier.erase(frontier.begin() + idx);

            if (grid.find(posKey(pos)) != grid.end()) continue;

            // Create combat room
            createRoom(nextId++, RoomType::Combat, pos);

            // Connect to adjacent rooms
            connectToNeighbors(pos);

            addNeighborsToFrontier(pos, frontier);
        }

        // Find furthest room from start and make it the exit
        int furthestId = findFurthestRoom(startPos);
        if (furthestId > 0) {
            for (auto& room : rooms) {
                if (room->getId() == furthestId) {
                    // Replace with exit room
                    GridPos exitPos = roomPositions[furthestId];
                    rooms.erase(std::remove_if(rooms.begin(), rooms.end(),
                        [furthestId](const auto& r) { return r->getId() == furthestId; }),
                        rooms.end());

                    createRoom(furthestId, RoomType::Exit, exitPos);
                    connectToNeighbors(exitPos);
                    break;
                }
            }
        }

        currentRoomId = 0;
    }

    Room* getCurrentRoom() {
        for (auto& room : rooms) {
            if (room->getId() == currentRoomId) {
                return room.get();
            }
        }
        return nullptr;
    }

    Room* getRoom(int id) {
        for (auto& room : rooms) {
            if (room->getId() == id) {
                return room.get();
            }
        }
        return nullptr;
    }

    bool transitionToRoom(int targetId, Direction fromDirection) {
        Room* target = getRoom(targetId);
        if (!target) return false;

        previousRoomId = currentRoomId;
        currentRoomId = targetId;
        entryDirection = fromDirection;
        return true;
    }

    sf::Vector2f getPlayerSpawnPosition() const {
        if (currentRoomId == 0) {
            return {roomSize.x / 2.f, roomSize.y / 2.f};
        }

        Room* room = nullptr;
        for (const auto& r : rooms) {
            if (r->getId() == currentRoomId) {
                room = r.get();
                break;
            }
        }
        if (room) {
            return room->getSpawnFromDoor(entryDirection);
        }
        return {roomSize.x / 2.f, roomSize.y / 2.f};
    }

    int getFloorNumber() const { return floorNumber; }
    int getCurrentRoomId() const { return currentRoomId; }
    const std::vector<std::unique_ptr<Room>>& getRooms() const { return rooms; }
    const std::map<int, GridPos>& getRoomPositions() const { return roomPositions; }

private:
    Room& createRoom(int id, RoomType type, GridPos pos) {
        auto room = std::make_unique<Room>(id, type, roomSize);
        Room& ref = *room;
        grid[posKey(pos)] = id;
        roomPositions[id] = pos;
        rooms.push_back(std::move(room));
        return ref;
    }

    void connectToNeighbors(GridPos pos) {
        Room* room = getRoom(grid[posKey(pos)]);
        if (!room) return;

        // Check each direction
        GridPos neighbors[] = {
            {pos.x, pos.y - 1},  // North
            {pos.x, pos.y + 1},  // South
            {pos.x + 1, pos.y},  // East
            {pos.x - 1, pos.y}   // West
        };
        Direction dirs[] = {Direction::North, Direction::South, Direction::East, Direction::West};
        Direction opposite[] = {Direction::South, Direction::North, Direction::West, Direction::East};

        for (int i = 0; i < 4; ++i) {
            auto it = grid.find(posKey(neighbors[i]));
            if (it != grid.end()) {
                int neighborId = it->second;
                room->connectDoor(dirs[i], neighborId);

                // Connect the other direction too
                Room* neighbor = getRoom(neighborId);
                if (neighbor) {
                    neighbor->connectDoor(opposite[i], room->getId());
                }
            }
        }
    }

    void addNeighborsToFrontier(GridPos pos, std::vector<GridPos>& frontier) {
        GridPos neighbors[] = {
            {pos.x, pos.y - 1},
            {pos.x, pos.y + 1},
            {pos.x + 1, pos.y},
            {pos.x - 1, pos.y}
        };

        for (const auto& n : neighbors) {
            if (grid.find(posKey(n)) == grid.end()) {
                // Check not already in frontier
                bool found = false;
                for (const auto& f : frontier) {
                    if (f == n) { found = true; break; }
                }
                if (!found) {
                    frontier.push_back(n);
                }
            }
        }
    }

    int findFurthestRoom(GridPos start) {
        int furthestId = 0;
        int maxDist = 0;

        for (const auto& [id, pos] : roomPositions) {
            int dist = std::abs(pos.x - start.x) + std::abs(pos.y - start.y);
            if (dist > maxDist) {
                maxDist = dist;
                furthestId = id;
            }
        }

        return furthestId;
    }

    static std::string posKey(GridPos pos) {
        return std::to_string(pos.x) + "," + std::to_string(pos.y);
    }

    int floorNumber;
    sf::Vector2f roomSize;
    std::vector<std::unique_ptr<Room>> rooms;
    std::map<std::string, int> grid;  // "x,y" -> roomId
    std::map<int, GridPos> roomPositions;  // roomId -> position

    int currentRoomId = 0;
    int previousRoomId = -1;
    Direction entryDirection = Direction::South;
};
