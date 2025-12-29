#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <any>

class EventBus {
public:
    static EventBus& instance() {
        static EventBus inst;
        return inst;
    }

    template<typename EventType>
    using Handler = std::function<void(const EventType&)>;

    template<typename EventType>
    void subscribe(Handler<EventType> handler) {
        auto& handlers = getHandlers<EventType>();
        handlers.push_back(std::move(handler));
    }

    template<typename EventType>
    void publish(const EventType& event) {
        auto& handlers = getHandlers<EventType>();
        for (auto& handler : handlers) {
            handler(event);
        }
    }

    template<typename EventType, typename... Args>
    void emit(Args&&... args) {
        publish(EventType{std::forward<Args>(args)...});
    }

    void clear() {
        handlers.clear();
    }

private:
    EventBus() = default;

    template<typename EventType>
    std::vector<Handler<EventType>>& getHandlers() {
        std::type_index typeIdx(typeid(EventType));
        auto it = handlers.find(typeIdx);
        if (it == handlers.end()) {
            handlers[typeIdx] = std::make_any<std::vector<Handler<EventType>>>();
            it = handlers.find(typeIdx);
        }
        return std::any_cast<std::vector<Handler<EventType>>&>(it->second);
    }

    std::unordered_map<std::type_index, std::any> handlers;

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
};

// Core game events
struct EnemyDiedEvent {
    unsigned int entityId;
    float x, y;
};

struct PlayerDamagedEvent {
    int amount;
    unsigned int sourceId;
};

struct PlayerDiedEvent {};

struct RoomClearedEvent {
    int roomId;
};

struct PickupCollectedEvent {
    unsigned int pickupId;
    int effectType;
    int value;
};

struct RoomEnteredEvent {
    int roomId;
};

struct FloorCompletedEvent {
    int floorNumber;
};
