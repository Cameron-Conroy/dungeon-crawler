#include <catch2/catch_all.hpp>
#include "ecs/Component.hpp"

// ============================================================================
// HealthComponent Tests
// ============================================================================

TEST_CASE("HealthComponent initial state", "[component][health]") {
    HealthComponent health(3, 0.5f);

    REQUIRE(health.current == 3);
    REQUIRE(health.max == 3);
    REQUIRE(health.isAlive());
    REQUIRE_FALSE(health.isInvincible());
}

TEST_CASE("HealthComponent taking damage", "[component][health]") {
    HealthComponent health(3, 0.5f);

    health.takeDamage(1);

    REQUIRE(health.current == 2);
    REQUIRE(health.isInvincible());
    REQUIRE(health.invincibilityTimer == Catch::Approx(0.5f));
}

TEST_CASE("HealthComponent invincibility blocks damage", "[component][health]") {
    HealthComponent health(3, 0.5f);

    health.takeDamage(1);  // Takes damage, becomes invincible
    health.takeDamage(1);  // Should be blocked

    REQUIRE(health.current == 2);  // Still 2, not 1
}

TEST_CASE("HealthComponent invincibility expires", "[component][health]") {
    HealthComponent health(3, 0.5f);

    health.takeDamage(1);
    REQUIRE(health.isInvincible());

    health.update(0.3f);
    REQUIRE(health.isInvincible());  // Still invincible

    health.update(0.3f);  // Total 0.6f, past 0.5f duration
    REQUIRE_FALSE(health.isInvincible());

    health.takeDamage(1);
    REQUIRE(health.current == 1);
}

TEST_CASE("HealthComponent death state", "[component][health]") {
    HealthComponent health(2, 0.0f);  // No invincibility for clean test

    REQUIRE(health.isAlive());

    health.takeDamage(1);
    REQUIRE(health.isAlive());

    health.takeDamage(1);
    REQUIRE_FALSE(health.isAlive());
}

TEST_CASE("HealthComponent overkill damage", "[component][health]") {
    HealthComponent health(3, 0.0f);

    health.takeDamage(10);

    REQUIRE(health.current == -7);
    REQUIRE_FALSE(health.isAlive());
}

// ============================================================================
// HurtboxComponent Tests
// ============================================================================

TEST_CASE("HurtboxComponent getBounds centered", "[component][collision]") {
    HurtboxComponent hurtbox({32.f, 32.f});
    sf::Vector2f pos{100.f, 100.f};

    auto bounds = hurtbox.getBounds(pos);

    REQUIRE(bounds.position.x == Catch::Approx(84.f));  // 100 - 16
    REQUIRE(bounds.position.y == Catch::Approx(84.f));
    REQUIRE(bounds.size.x == Catch::Approx(32.f));
    REQUIRE(bounds.size.y == Catch::Approx(32.f));
}

TEST_CASE("HurtboxComponent getBounds with offset", "[component][collision]") {
    HurtboxComponent hurtbox({20.f, 20.f});
    hurtbox.offset = {5.f, -5.f};
    sf::Vector2f pos{50.f, 50.f};

    auto bounds = hurtbox.getBounds(pos);

    REQUIRE(bounds.position.x == Catch::Approx(45.f));  // 50 - 10 + 5
    REQUIRE(bounds.position.y == Catch::Approx(35.f));  // 50 - 10 - 5
}

// ============================================================================
// HitboxComponent Tests
// ============================================================================

TEST_CASE("HitboxComponent getBounds with facing right", "[component][collision]") {
    HitboxComponent hitbox({40.f, 20.f}, 1, Faction::Player);
    hitbox.facing = {1.f, 0.f};  // Facing right
    sf::Vector2f pos{100.f, 100.f};

    auto bounds = hitbox.getBounds(pos);

    // Attack position should be offset in facing direction
    REQUIRE(bounds.position.x > 100.f);
    REQUIRE(bounds.size.x == Catch::Approx(40.f));
    REQUIRE(bounds.size.y == Catch::Approx(20.f));
}

TEST_CASE("HitboxComponent getBounds with facing left", "[component][collision]") {
    HitboxComponent hitbox({40.f, 20.f}, 1, Faction::Player);
    hitbox.facing = {-1.f, 0.f};  // Facing left
    sf::Vector2f pos{100.f, 100.f};

    auto bounds = hitbox.getBounds(pos);

    // Attack position should be offset left
    REQUIRE(bounds.position.x < 100.f);
}

TEST_CASE("HitboxComponent faction assignment", "[component][collision]") {
    HitboxComponent playerHitbox({40.f, 20.f}, 1, Faction::Player);
    HitboxComponent enemyHitbox({30.f, 30.f}, 2, Faction::Enemy);

    REQUIRE(playerHitbox.faction == Faction::Player);
    REQUIRE(enemyHitbox.faction == Faction::Enemy);
    REQUIRE(playerHitbox.damage == 1);
    REQUIRE(enemyHitbox.damage == 2);
}

// ============================================================================
// PhysicsComponent Tests
// ============================================================================

TEST_CASE("PhysicsComponent default values", "[component][physics]") {
    PhysicsComponent physics;

    REQUIRE(physics.velocity.x == 0.f);
    REQUIRE(physics.velocity.y == 0.f);
    REQUIRE(physics.friction == 0.f);
    REQUIRE(physics.speed == 100.f);
    REQUIRE(physics.clampToRoom == true);
}

TEST_CASE("PhysicsComponent constructor with speed", "[component][physics]") {
    PhysicsComponent physics(200.f);

    REQUIRE(physics.speed == 200.f);
}

// ============================================================================
// AIComponent Tests
// ============================================================================

TEST_CASE("AIComponent default behavior", "[component][ai]") {
    AIComponent ai;

    REQUIRE(ai.behavior == AIBehavior::Wander);
    REQUIRE(ai.isChasing == false);
    REQUIRE(ai.detectionRadius == 150.f);
    REQUIRE(ai.loseRadius == 200.f);
}

TEST_CASE("AIComponent custom configuration", "[component][ai]") {
    AIComponent ai(AIBehavior::Erratic, 100.f, 50.f, 120.f);

    REQUIRE(ai.behavior == AIBehavior::Erratic);
    REQUIRE(ai.detectionRadius == 100.f);
    REQUIRE(ai.wanderSpeed == 50.f);
    REQUIRE(ai.chaseSpeed == 120.f);
}
