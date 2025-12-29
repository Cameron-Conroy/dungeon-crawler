#include <catch2/catch_all.hpp>
#include "core/EventBus.hpp"

TEST_CASE("EventBus subscribe and publish", "[eventbus]") {
    EventBus::instance().clear();

    bool received = false;
    unsigned int receivedId = 0;
    float receivedX = 0.f;

    EventBus::instance().subscribe<EnemyDiedEvent>([&](const EnemyDiedEvent& e) {
        received = true;
        receivedId = e.entityId;
        receivedX = e.x;
    });

    EventBus::instance().emit<EnemyDiedEvent>(42u, 100.f, 200.f);

    REQUIRE(received);
    REQUIRE(receivedId == 42);
    REQUIRE(receivedX == Catch::Approx(100.f));
}

TEST_CASE("EventBus multiple subscribers", "[eventbus]") {
    EventBus::instance().clear();

    int callCount = 0;

    EventBus::instance().subscribe<PlayerDiedEvent>([&](const PlayerDiedEvent&) {
        callCount++;
    });
    EventBus::instance().subscribe<PlayerDiedEvent>([&](const PlayerDiedEvent&) {
        callCount++;
    });
    EventBus::instance().subscribe<PlayerDiedEvent>([&](const PlayerDiedEvent&) {
        callCount++;
    });

    EventBus::instance().emit<PlayerDiedEvent>();

    REQUIRE(callCount == 3);
}

TEST_CASE("EventBus type isolation", "[eventbus]") {
    EventBus::instance().clear();

    bool wrongTypeCalled = false;
    bool correctTypeCalled = false;

    EventBus::instance().subscribe<PlayerDiedEvent>([&](const PlayerDiedEvent&) {
        wrongTypeCalled = true;
    });
    EventBus::instance().subscribe<RoomClearedEvent>([&](const RoomClearedEvent&) {
        correctTypeCalled = true;
    });

    EventBus::instance().emit<RoomClearedEvent>(1);

    REQUIRE_FALSE(wrongTypeCalled);
    REQUIRE(correctTypeCalled);
}

TEST_CASE("EventBus clear removes all handlers", "[eventbus]") {
    EventBus::instance().clear();

    bool called = false;

    EventBus::instance().subscribe<PlayerDiedEvent>([&](const PlayerDiedEvent&) {
        called = true;
    });

    EventBus::instance().clear();
    EventBus::instance().emit<PlayerDiedEvent>();

    REQUIRE_FALSE(called);
}

TEST_CASE("EventBus publish with event data", "[eventbus]") {
    EventBus::instance().clear();

    int receivedRoomId = -1;

    EventBus::instance().subscribe<RoomClearedEvent>([&](const RoomClearedEvent& e) {
        receivedRoomId = e.roomId;
    });

    RoomClearedEvent event;
    event.roomId = 42;
    EventBus::instance().publish(event);

    REQUIRE(receivedRoomId == 42);
}

TEST_CASE("EventBus PickupCollectedEvent", "[eventbus]") {
    EventBus::instance().clear();

    unsigned int pickupId = 0;
    int effectType = -1;
    int value = 0;

    EventBus::instance().subscribe<PickupCollectedEvent>([&](const PickupCollectedEvent& e) {
        pickupId = e.pickupId;
        effectType = e.effectType;
        value = e.value;
    });

    EventBus::instance().emit<PickupCollectedEvent>(123u, 0, 5);

    REQUIRE(pickupId == 123);
    REQUIRE(effectType == 0);
    REQUIRE(value == 5);
}
