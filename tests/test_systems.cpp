#include <catch2/catch_all.hpp>
#include "ecs/EntityManager.hpp"
#include "ecs/EntityFactory.hpp"
#include "ecs/Systems.hpp"
#include "core/EventBus.hpp"

// ============================================================================
// PhysicsSystem Tests
// ============================================================================

TEST_CASE("PhysicsSystem applies velocity", "[system][physics]") {
    EntityManager manager;
    PhysicsSystem physics;

    auto& entity = manager.createEntity();
    entity.position = {100.f, 100.f};
    auto& phys = entity.addComponent<PhysicsComponent>();
    phys.velocity = {100.f, 50.f};
    phys.clampToRoom = false;

    physics.update(manager, 1.0f);

    REQUIRE(entity.position.x == Catch::Approx(200.f));
    REQUIRE(entity.position.y == Catch::Approx(150.f));
}

TEST_CASE("PhysicsSystem applies velocity with dt", "[system][physics]") {
    EntityManager manager;
    PhysicsSystem physics;

    auto& entity = manager.createEntity();
    entity.position = {0.f, 0.f};
    auto& phys = entity.addComponent<PhysicsComponent>();
    phys.velocity = {100.f, 0.f};
    phys.clampToRoom = false;

    physics.update(manager, 0.5f);

    REQUIRE(entity.position.x == Catch::Approx(50.f));
}

TEST_CASE("PhysicsSystem room clamping left edge", "[system][physics]") {
    EntityManager manager;
    PhysicsSystem physics;

    auto& entity = manager.createEntity();
    entity.position = {20.f, 100.f};
    auto& phys = entity.addComponent<PhysicsComponent>();
    phys.velocity = {-100.f, 0.f};
    phys.clampToRoom = true;
    phys.roomBounds = sf::FloatRect({0.f, 0.f}, {800.f, 600.f});

    physics.update(manager, 1.0f);

    // Should be clamped at left edge (halfSize=16)
    REQUIRE(entity.position.x >= 16.f);
}

TEST_CASE("PhysicsSystem room clamping all edges", "[system][physics]") {
    EntityManager manager;
    PhysicsSystem physics;

    sf::FloatRect bounds({0.f, 0.f}, {100.f, 100.f});

    // Test left edge
    auto& e1 = manager.createEntity();
    e1.position = {-50.f, 50.f};
    e1.addComponent<PhysicsComponent>().roomBounds = bounds;

    // Test right edge
    auto& e2 = manager.createEntity();
    e2.position = {150.f, 50.f};
    e2.addComponent<PhysicsComponent>().roomBounds = bounds;

    // Test top edge
    auto& e3 = manager.createEntity();
    e3.position = {50.f, -50.f};
    e3.addComponent<PhysicsComponent>().roomBounds = bounds;

    // Test bottom edge
    auto& e4 = manager.createEntity();
    e4.position = {50.f, 150.f};
    e4.addComponent<PhysicsComponent>().roomBounds = bounds;

    physics.update(manager, 0.f);

    REQUIRE(e1.position.x >= 16.f);
    REQUIRE(e2.position.x <= 84.f);
    REQUIRE(e3.position.y >= 16.f);
    REQUIRE(e4.position.y <= 84.f);
}

TEST_CASE("PhysicsSystem skips entities without PhysicsComponent", "[system][physics]") {
    EntityManager manager;
    PhysicsSystem physics;

    auto& entity = manager.createEntity();
    entity.position = {100.f, 100.f};
    // No PhysicsComponent added

    physics.update(manager, 1.0f);

    // Position should be unchanged
    REQUIRE(entity.position.x == Catch::Approx(100.f));
    REQUIRE(entity.position.y == Catch::Approx(100.f));
}

// ============================================================================
// HealthSystem Tests
// ============================================================================

TEST_CASE("HealthSystem updates invincibility timers", "[system][health]") {
    EntityManager manager;
    HealthSystem healthSys;

    auto& entity = manager.createEntity();
    auto& health = entity.addComponent<HealthComponent>(3, 1.0f);
    health.takeDamage(1);

    REQUIRE(health.isInvincible());
    REQUIRE(health.invincibilityTimer == Catch::Approx(1.0f));

    healthSys.update(manager, 0.5f);
    REQUIRE(health.invincibilityTimer == Catch::Approx(0.5f));
    REQUIRE(health.isInvincible());

    healthSys.update(manager, 0.6f);
    REQUIRE_FALSE(health.isInvincible());
}

TEST_CASE("HealthSystem updates multiple entities", "[system][health]") {
    EntityManager manager;
    HealthSystem healthSys;

    auto& e1 = manager.createEntity();
    auto& h1 = e1.addComponent<HealthComponent>(3, 1.0f);
    h1.takeDamage(1);

    auto& e2 = manager.createEntity();
    auto& h2 = e2.addComponent<HealthComponent>(3, 0.5f);
    h2.takeDamage(1);

    healthSys.update(manager, 0.6f);

    REQUIRE(h1.isInvincible());      // 1.0 - 0.6 = 0.4 > 0
    REQUIRE_FALSE(h2.isInvincible()); // 0.5 - 0.6 < 0
}

// ============================================================================
// CollisionSystem Tests
// ============================================================================

TEST_CASE("CollisionSystem hitbox-hurtbox collision", "[system][collision]") {
    EntityManager manager;
    CollisionSystem collision;
    EventBus::instance().clear();

    // Create attacker with active hitbox
    auto& attacker = manager.createEntity();
    attacker.position = {100.f, 100.f};
    auto& hitbox = attacker.addComponent<HitboxComponent>();
    hitbox.size = {40.f, 20.f};
    hitbox.damage = 1;
    hitbox.faction = Faction::Player;
    hitbox.active = true;
    hitbox.facing = {1.f, 0.f};

    // Create target (enemy) close enough to be hit
    auto& target = manager.createEntity();
    target.position = {140.f, 100.f};
    target.addComponent<HurtboxComponent>(sf::Vector2f{32.f, 32.f});
    target.addComponent<HealthComponent>(2, 0.f);
    target.addComponent<EnemyTag>();

    collision.update(manager);

    auto* health = target.getComponent<HealthComponent>();
    REQUIRE(health->current == 1);  // Took 1 damage
}

TEST_CASE("CollisionSystem inactive hitbox does nothing", "[system][collision]") {
    EntityManager manager;
    CollisionSystem collision;
    EventBus::instance().clear();

    auto& attacker = manager.createEntity();
    attacker.position = {100.f, 100.f};
    auto& hitbox = attacker.addComponent<HitboxComponent>();
    hitbox.active = false;  // Not active

    auto& target = manager.createEntity();
    target.position = {100.f, 100.f};
    target.addComponent<HurtboxComponent>(sf::Vector2f{32.f, 32.f});
    target.addComponent<HealthComponent>(3, 0.f);

    collision.update(manager);

    auto* health = target.getComponent<HealthComponent>();
    REQUIRE(health->current == 3);  // No damage
}

TEST_CASE("CollisionSystem same faction immunity", "[system][collision]") {
    EntityManager manager;
    CollisionSystem collision;
    EventBus::instance().clear();

    // Two player-faction entities
    auto& e1 = manager.createEntity();
    e1.position = {100.f, 100.f};
    auto& hitbox = e1.addComponent<HitboxComponent>();
    hitbox.active = true;
    hitbox.faction = Faction::Player;
    hitbox.facing = {1.f, 0.f};

    auto& e2 = manager.createEntity();
    e2.position = {120.f, 100.f};
    e2.addComponent<HurtboxComponent>(sf::Vector2f{32.f, 32.f});
    e2.addComponent<HealthComponent>(3);
    e2.addComponent<HitboxComponent>().faction = Faction::Player;

    collision.update(manager);

    auto* health = e2.getComponent<HealthComponent>();
    REQUIRE(health->current == 3);  // No damage taken
}

TEST_CASE("CollisionSystem invincible target not damaged", "[system][collision]") {
    EntityManager manager;
    CollisionSystem collision;
    EventBus::instance().clear();

    auto& attacker = manager.createEntity();
    attacker.position = {100.f, 100.f};
    auto& hitbox = attacker.addComponent<HitboxComponent>();
    hitbox.active = true;
    hitbox.faction = Faction::Player;
    hitbox.facing = {1.f, 0.f};

    auto& target = manager.createEntity();
    target.position = {120.f, 100.f};
    target.addComponent<HurtboxComponent>(sf::Vector2f{32.f, 32.f});
    auto& health = target.addComponent<HealthComponent>(3, 1.0f);
    health.takeDamage(1);  // Makes target invincible

    REQUIRE(health.current == 2);
    REQUIRE(health.isInvincible());

    collision.update(manager);

    REQUIRE(health.current == 2);  // Still 2, invincibility blocked damage
}

// ============================================================================
// PickupSystem Tests
// ============================================================================

TEST_CASE("PickupSystem health collection", "[system][pickup]") {
    EntityManager manager;
    PickupSystem pickupSys;
    EventBus::instance().clear();

    // Create player at low health
    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});
    auto& player = EntityFactory::createPlayer(manager, {100.f, 100.f}, bounds);
    player.getComponent<HealthComponent>()->current = 1;

    // Create health pickup overlapping player
    auto& pickup = EntityFactory::createHealthPickup(manager, {100.f, 100.f});

    pickupSys.update(manager);

    REQUIRE(player.getComponent<HealthComponent>()->current == 2);
    REQUIRE(pickup.getComponent<PickupComponent>()->collected == true);
    REQUIRE(pickup.active == false);
}

TEST_CASE("PickupSystem health capped at max", "[system][pickup]") {
    EntityManager manager;
    PickupSystem pickupSys;
    EventBus::instance().clear();

    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});
    auto& player = EntityFactory::createPlayer(manager, {100.f, 100.f}, bounds);
    // Player starts at max health (3)

    auto& pickup = EntityFactory::createHealthPickup(manager, {100.f, 100.f});

    pickupSys.update(manager);

    REQUIRE(player.getComponent<HealthComponent>()->current == 3);  // Still max
    REQUIRE(pickup.getComponent<PickupComponent>()->collected == true);
}

TEST_CASE("PickupSystem no collection without player", "[system][pickup]") {
    EntityManager manager;
    PickupSystem pickupSys;
    EventBus::instance().clear();

    // Only pickup, no player
    auto& pickup = EntityFactory::createHealthPickup(manager, {100.f, 100.f});

    pickupSys.update(manager);

    REQUIRE_FALSE(pickup.getComponent<PickupComponent>()->collected);
    REQUIRE(pickup.active == true);
}

TEST_CASE("PickupSystem emits event on collection", "[system][pickup]") {
    EntityManager manager;
    PickupSystem pickupSys;
    EventBus::instance().clear();

    bool eventReceived = false;
    int receivedValue = 0;

    EventBus::instance().subscribe<PickupCollectedEvent>([&](const PickupCollectedEvent& e) {
        eventReceived = true;
        receivedValue = e.value;
    });

    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});
    EntityFactory::createPlayer(manager, {100.f, 100.f}, bounds);
    EntityFactory::createHealthPickup(manager, {100.f, 100.f});

    pickupSys.update(manager);

    REQUIRE(eventReceived);
    REQUIRE(receivedValue == 1);
}
