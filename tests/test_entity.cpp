#include <catch2/catch_all.hpp>
#include "ecs/Entity.hpp"
#include "ecs/EntityManager.hpp"
#include "ecs/Component.hpp"

// ============================================================================
// Entity Tests
// ============================================================================

TEST_CASE("Entity has unique ID", "[entity]") {
    Entity e1(1);
    Entity e2(2);

    REQUIRE(e1.getId() == 1);
    REQUIRE(e2.getId() == 2);
    REQUIRE(e1.getId() != e2.getId());
}

TEST_CASE("Entity default state", "[entity]") {
    Entity entity(1);

    REQUIRE(entity.active == true);
    REQUIRE(entity.position.x == 0.f);
    REQUIRE(entity.position.y == 0.f);
}

TEST_CASE("Entity add and retrieve component", "[entity]") {
    Entity entity(1);

    entity.addComponent<HealthComponent>(5, 1.0f);
    auto* health = entity.getComponent<HealthComponent>();

    REQUIRE(health != nullptr);
    REQUIRE(health->current == 5);
    REQUIRE(health->max == 5);
}

TEST_CASE("Entity hasComponent returns correct state", "[entity]") {
    Entity entity(1);

    REQUIRE_FALSE(entity.hasComponent<HealthComponent>());

    entity.addComponent<HealthComponent>();

    REQUIRE(entity.hasComponent<HealthComponent>());
    REQUIRE_FALSE(entity.hasComponent<PhysicsComponent>());
}

TEST_CASE("Entity removeComponent", "[entity]") {
    Entity entity(1);

    entity.addComponent<HealthComponent>();
    REQUIRE(entity.hasComponent<HealthComponent>());

    entity.removeComponent<HealthComponent>();
    REQUIRE_FALSE(entity.hasComponent<HealthComponent>());
}

TEST_CASE("Entity getComponent returns nullptr for missing", "[entity]") {
    Entity entity(1);

    auto* health = entity.getComponent<HealthComponent>();

    REQUIRE(health == nullptr);
}

TEST_CASE("Entity multiple component types", "[entity]") {
    Entity entity(1);

    entity.addComponent<HealthComponent>(3);
    entity.addComponent<PhysicsComponent>(100.f);
    entity.addComponent<SpriteComponent>();

    REQUIRE(entity.hasComponent<HealthComponent>());
    REQUIRE(entity.hasComponent<PhysicsComponent>());
    REQUIRE(entity.hasComponent<SpriteComponent>());
    REQUIRE_FALSE(entity.hasComponent<AIComponent>());

    auto* physics = entity.getComponent<PhysicsComponent>();
    REQUIRE(physics->speed == 100.f);
}

TEST_CASE("Entity addComponent overwrites existing", "[entity]") {
    Entity entity(1);

    entity.addComponent<HealthComponent>(3);
    entity.addComponent<HealthComponent>(5);

    auto* health = entity.getComponent<HealthComponent>();
    REQUIRE(health->current == 5);
}

// ============================================================================
// EntityManager Tests
// ============================================================================

TEST_CASE("EntityManager createEntity assigns unique IDs", "[entitymanager]") {
    EntityManager manager;

    auto& e1 = manager.createEntity();
    auto& e2 = manager.createEntity();
    auto& e3 = manager.createEntity();

    REQUIRE(e1.getId() != e2.getId());
    REQUIRE(e2.getId() != e3.getId());
    REQUIRE(e1.getId() != e3.getId());
}

TEST_CASE("EntityManager count", "[entitymanager]") {
    EntityManager manager;

    REQUIRE(manager.count() == 0);

    manager.createEntity();
    REQUIRE(manager.count() == 1);

    manager.createEntity();
    manager.createEntity();
    REQUIRE(manager.count() == 3);
}

TEST_CASE("EntityManager countWith", "[entitymanager]") {
    EntityManager manager;

    manager.createEntity().addComponent<HealthComponent>();
    manager.createEntity().addComponent<HealthComponent>();
    manager.createEntity().addComponent<PhysicsComponent>();

    REQUIRE(manager.count() == 3);
    REQUIRE(manager.countWith<HealthComponent>() == 2);
    REQUIRE(manager.countWith<PhysicsComponent>() == 1);
    REQUIRE(manager.countWith<AIComponent>() == 0);
}

TEST_CASE("EntityManager getEntity", "[entitymanager]") {
    EntityManager manager;

    auto& created = manager.createEntity();
    unsigned int id = created.getId();

    auto* retrieved = manager.getEntity(id);

    REQUIRE(retrieved != nullptr);
    REQUIRE(retrieved->getId() == id);
}

TEST_CASE("EntityManager getEntity returns nullptr for invalid ID", "[entitymanager]") {
    EntityManager manager;

    manager.createEntity();

    auto* retrieved = manager.getEntity(999);

    REQUIRE(retrieved == nullptr);
}

TEST_CASE("EntityManager destroyEntity marks inactive", "[entitymanager]") {
    EntityManager manager;

    auto& entity = manager.createEntity();
    unsigned int id = entity.getId();

    REQUIRE(entity.active == true);

    manager.destroyEntity(id);

    REQUIRE(entity.active == false);
    REQUIRE(manager.count() == 1);  // Still there, just inactive
}

TEST_CASE("EntityManager cleanup removes inactive", "[entitymanager]") {
    EntityManager manager;

    auto& e1 = manager.createEntity();
    manager.createEntity();

    manager.destroyEntity(e1.getId());
    REQUIRE(manager.count() == 2);

    manager.cleanup();
    REQUIRE(manager.count() == 1);
}

TEST_CASE("EntityManager clear", "[entitymanager]") {
    EntityManager manager;

    manager.createEntity();
    manager.createEntity();
    manager.createEntity();

    REQUIRE(manager.count() == 3);

    manager.clear();

    REQUIRE(manager.count() == 0);
}

TEST_CASE("EntityManager forEach", "[entitymanager]") {
    EntityManager manager;

    manager.createEntity().position = {1.f, 0.f};
    manager.createEntity().position = {2.f, 0.f};
    manager.createEntity().position = {3.f, 0.f};

    float sum = 0.f;
    manager.forEach([&sum](Entity& e) {
        sum += e.position.x;
    });

    REQUIRE(sum == Catch::Approx(6.f));
}

TEST_CASE("EntityManager forEach skips inactive", "[entitymanager]") {
    EntityManager manager;

    auto& e1 = manager.createEntity();
    e1.position = {1.f, 0.f};
    auto& e2 = manager.createEntity();
    e2.position = {2.f, 0.f};

    manager.destroyEntity(e1.getId());

    float sum = 0.f;
    manager.forEach([&sum](Entity& e) {
        sum += e.position.x;
    });

    REQUIRE(sum == Catch::Approx(2.f));
}

TEST_CASE("EntityManager forEachWith single component", "[entitymanager]") {
    EntityManager manager;

    manager.createEntity().addComponent<HealthComponent>();
    manager.createEntity().addComponent<PhysicsComponent>();
    manager.createEntity().addComponent<HealthComponent>();

    int healthCount = 0;
    manager.forEachWith<HealthComponent>([&healthCount](Entity&) {
        healthCount++;
    });

    REQUIRE(healthCount == 2);
}

TEST_CASE("EntityManager forEachWith two components", "[entitymanager]") {
    EntityManager manager;

    auto& e1 = manager.createEntity();
    e1.addComponent<HealthComponent>();
    e1.addComponent<PhysicsComponent>();

    auto& e2 = manager.createEntity();
    e2.addComponent<HealthComponent>();

    auto& e3 = manager.createEntity();
    e3.addComponent<PhysicsComponent>();

    int bothCount = 0;
    manager.forEachWith<HealthComponent, PhysicsComponent>([&bothCount](Entity&) {
        bothCount++;
    });

    REQUIRE(bothCount == 1);
}

TEST_CASE("EntityManager forEachWith three components", "[entitymanager]") {
    EntityManager manager;

    auto& e1 = manager.createEntity();
    e1.addComponent<HealthComponent>();
    e1.addComponent<PhysicsComponent>();
    e1.addComponent<SpriteComponent>();

    auto& e2 = manager.createEntity();
    e2.addComponent<HealthComponent>();
    e2.addComponent<PhysicsComponent>();

    int allThreeCount = 0;
    manager.forEachWith<HealthComponent, PhysicsComponent, SpriteComponent>([&allThreeCount](Entity&) {
        allThreeCount++;
    });

    REQUIRE(allThreeCount == 1);
}
