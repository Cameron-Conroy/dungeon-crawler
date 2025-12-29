#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Base component class
class Component {
public:
    virtual ~Component() = default;
};

// Rendering
struct SpriteComponent : Component {
    std::string textureId;
    sf::Vector2f size{32.f, 32.f};
    sf::Color color{sf::Color::White};
    sf::Vector2f origin{0.f, 0.f};  // Offset from center
    bool useTexture = false;
    int animationFrame = 0;

    SpriteComponent() = default;
    SpriteComponent(sf::Vector2f size, sf::Color color)
        : size(size), color(color), origin(size.x / 2.f, size.y / 2.f) {}
};

// Physics
struct PhysicsComponent : Component {
    sf::Vector2f velocity{0.f, 0.f};
    float friction = 0.f;
    float speed = 100.f;
    bool clampToRoom = true;
    sf::FloatRect roomBounds;

    PhysicsComponent() = default;
    explicit PhysicsComponent(float speed) : speed(speed) {}
};

// Health
struct HealthComponent : Component {
    int current = 3;
    int max = 3;
    float invincibilityTimer = 0.f;
    float invincibilityDuration = 0.5f;

    HealthComponent() = default;
    HealthComponent(int hp, float invincDuration = 0.5f)
        : current(hp), max(hp), invincibilityDuration(invincDuration) {}

    bool isInvincible() const { return invincibilityTimer > 0.f; }
    bool isAlive() const { return current > 0; }

    void takeDamage(int amount) {
        if (!isInvincible()) {
            current -= amount;
            invincibilityTimer = invincibilityDuration;
        }
    }

    void update(float dt) {
        if (invincibilityTimer > 0.f) {
            invincibilityTimer -= dt;
        }
    }
};

// Collision detection - receiving damage
struct HurtboxComponent : Component {
    sf::Vector2f size{32.f, 32.f};
    sf::Vector2f offset{0.f, 0.f};

    HurtboxComponent() = default;
    explicit HurtboxComponent(sf::Vector2f size) : size(size) {}

    sf::FloatRect getBounds(sf::Vector2f entityPos) const {
        return sf::FloatRect(
            {entityPos.x - size.x / 2.f + offset.x, entityPos.y - size.y / 2.f + offset.y},
            size
        );
    }
};

// Collision detection - dealing damage
enum class Faction { Player, Enemy, Neutral };

struct HitboxComponent : Component {
    sf::Vector2f size{40.f, 20.f};
    sf::Vector2f offset{0.f, 0.f};
    int damage = 1;
    Faction faction = Faction::Player;
    bool active = false;
    sf::Vector2f facing{1.f, 0.f};

    HitboxComponent() = default;
    HitboxComponent(sf::Vector2f size, int damage, Faction faction)
        : size(size), damage(damage), faction(faction) {}

    sf::FloatRect getBounds(sf::Vector2f entityPos) const {
        sf::Vector2f attackPos = entityPos + facing * (size.x / 2.f + 16.f);
        return sf::FloatRect(
            {attackPos.x - size.x / 2.f + offset.x, attackPos.y - size.y / 2.f + offset.y},
            size
        );
    }
};

// AI Behavior
enum class AIBehavior { Wander, Chase, Erratic };

struct AIComponent : Component {
    AIBehavior behavior = AIBehavior::Wander;
    float detectionRadius = 150.f;
    float loseRadius = 200.f;
    float wanderSpeed = 40.f;
    float chaseSpeed = 80.f;
    float wanderTimer = 0.f;
    float directionChangeInterval = 1.f;
    bool isChasing = false;

    AIComponent() = default;
    AIComponent(AIBehavior behavior, float detectRadius, float wanderSpd, float chaseSpd)
        : behavior(behavior), detectionRadius(detectRadius),
          wanderSpeed(wanderSpd), chaseSpeed(chaseSpd) {}
};

// Player control marker
struct PlayerControlComponent : Component {
    float attackDuration = 0.15f;
    float attackCooldown = 0.3f;
    float attackTimer = 0.f;
    float cooldownTimer = 0.f;
    bool isAttacking = false;
    sf::Vector2f facing{1.f, 0.f};

    PlayerControlComponent() = default;
};

// Pickup effects
enum class PickupType { Health, SpeedBoost, DamageUp };

struct PickupComponent : Component {
    PickupType type = PickupType::Health;
    int value = 1;
    bool collected = false;

    PickupComponent() = default;
    PickupComponent(PickupType type, int value) : type(type), value(value) {}
};

// Tag for enemies (used by room to track clear state)
struct EnemyTag : Component {};

// Tag for pickups
struct PickupTag : Component {};
