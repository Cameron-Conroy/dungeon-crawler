#include <catch2/catch_all.hpp>
#include "ecs/EntityManager.hpp"
#include "ecs/EntityFactory.hpp"
#include "ecs/Component.hpp"

TEST_CASE("EntityFactory createPlayer has all components", "[factory]") {
    EntityManager manager;
    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});

    auto& player = EntityFactory::createPlayer(manager, {400.f, 300.f}, bounds);

    REQUIRE(player.position.x == Catch::Approx(400.f));
    REQUIRE(player.position.y == Catch::Approx(300.f));

    REQUIRE(player.hasComponent<SpriteComponent>());
    REQUIRE(player.hasComponent<PhysicsComponent>());
    REQUIRE(player.hasComponent<HealthComponent>());
    REQUIRE(player.hasComponent<HurtboxComponent>());
    REQUIRE(player.hasComponent<HitboxComponent>());
    REQUIRE(player.hasComponent<PlayerControlComponent>());
}

TEST_CASE("EntityFactory createPlayer correct values", "[factory]") {
    EntityManager manager;
    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});

    auto& player = EntityFactory::createPlayer(manager, {400.f, 300.f}, bounds);

    auto* health = player.getComponent<HealthComponent>();
    REQUIRE(health->current == 3);
    REQUIRE(health->max == 3);

    auto* physics = player.getComponent<PhysicsComponent>();
    REQUIRE(physics->speed == 120.f);
    REQUIRE(physics->clampToRoom == true);

    auto* hitbox = player.getComponent<HitboxComponent>();
    REQUIRE(hitbox->faction == Faction::Player);
    REQUIRE_FALSE(hitbox->active);
    REQUIRE(hitbox->damage == 1);

    auto* sprite = player.getComponent<SpriteComponent>();
    REQUIRE(sprite->size.x == Catch::Approx(32.f));
    REQUIRE(sprite->size.y == Catch::Approx(32.f));
}

TEST_CASE("EntityFactory createEnemy Slime", "[factory]") {
    EntityManager manager;
    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});

    auto& enemy = EntityFactory::createEnemy(
        manager,
        EntityFactory::EnemyType::Slime,
        {100.f, 100.f},
        bounds
    );

    REQUIRE(enemy.position.x == Catch::Approx(100.f));
    REQUIRE(enemy.position.y == Catch::Approx(100.f));

    REQUIRE(enemy.hasComponent<EnemyTag>());
    REQUIRE(enemy.hasComponent<AIComponent>());
    REQUIRE(enemy.hasComponent<HealthComponent>());
    REQUIRE(enemy.hasComponent<HurtboxComponent>());
    REQUIRE(enemy.hasComponent<SpriteComponent>());
    REQUIRE(enemy.hasComponent<PhysicsComponent>());

    auto* ai = enemy.getComponent<AIComponent>();
    REQUIRE(ai->behavior == AIBehavior::Wander);
    REQUIRE(ai->wanderSpeed == 40.f);
    REQUIRE(ai->chaseSpeed == 80.f);

    auto* health = enemy.getComponent<HealthComponent>();
    REQUIRE(health->current == 1);
}

TEST_CASE("EntityFactory createEnemy Bat", "[factory]") {
    EntityManager manager;
    sf::FloatRect bounds({0.f, 0.f}, {800.f, 600.f});

    auto& enemy = EntityFactory::createEnemy(
        manager,
        EntityFactory::EnemyType::Bat,
        {200.f, 200.f},
        bounds
    );

    REQUIRE(enemy.hasComponent<EnemyTag>());

    auto* ai = enemy.getComponent<AIComponent>();
    REQUIRE(ai->behavior == AIBehavior::Erratic);
    REQUIRE(ai->wanderSpeed == 60.f);
    REQUIRE(ai->chaseSpeed == 100.f);

    auto* sprite = enemy.getComponent<SpriteComponent>();
    REQUIRE(sprite->size.x == Catch::Approx(24.f));
    REQUIRE(sprite->size.y == Catch::Approx(24.f));
}

TEST_CASE("EntityFactory createHealthPickup", "[factory]") {
    EntityManager manager;

    auto& pickup = EntityFactory::createHealthPickup(manager, {200.f, 200.f});

    REQUIRE(pickup.position.x == Catch::Approx(200.f));
    REQUIRE(pickup.position.y == Catch::Approx(200.f));

    REQUIRE(pickup.hasComponent<PickupComponent>());
    REQUIRE(pickup.hasComponent<PickupTag>());
    REQUIRE(pickup.hasComponent<HurtboxComponent>());
    REQUIRE(pickup.hasComponent<SpriteComponent>());

    auto* pickupComp = pickup.getComponent<PickupComponent>();
    REQUIRE(pickupComp->type == PickupType::Health);
    REQUIRE(pickupComp->value == 1);
    REQUIRE_FALSE(pickupComp->collected);

    auto* sprite = pickup.getComponent<SpriteComponent>();
    REQUIRE(sprite->size.x == Catch::Approx(16.f));
    REQUIRE(sprite->size.y == Catch::Approx(16.f));
}

TEST_CASE("EntityFactory room bounds propagation", "[factory]") {
    EntityManager manager;
    sf::FloatRect bounds({50.f, 50.f}, {700.f, 500.f});

    auto& player = EntityFactory::createPlayer(manager, {100.f, 100.f}, bounds);
    auto& enemy = EntityFactory::createEnemy(
        manager,
        EntityFactory::EnemyType::Slime,
        {200.f, 200.f},
        bounds
    );

    auto* playerPhysics = player.getComponent<PhysicsComponent>();
    REQUIRE(playerPhysics->roomBounds.position.x == Catch::Approx(50.f));
    REQUIRE(playerPhysics->roomBounds.size.x == Catch::Approx(700.f));

    auto* enemyPhysics = enemy.getComponent<PhysicsComponent>();
    REQUIRE(enemyPhysics->roomBounds.position.x == Catch::Approx(50.f));
    REQUIRE(enemyPhysics->roomBounds.size.x == Catch::Approx(700.f));
}
