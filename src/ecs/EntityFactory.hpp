#pragma once

#include "EntityManager.hpp"
#include "Component.hpp"

namespace EntityFactory {

inline Entity& createPlayer(EntityManager& manager, sf::Vector2f position, const sf::FloatRect& roomBounds) {
    Entity& player = manager.createEntity();
    player.position = position;

    // Sprite
    auto& sprite = player.addComponent<SpriteComponent>();
    sprite.size = {32.f, 32.f};
    sprite.origin = {16.f, 16.f};
    sprite.color = sf::Color::Green;

    // Physics
    auto& physics = player.addComponent<PhysicsComponent>();
    physics.speed = 120.f;
    physics.roomBounds = roomBounds;
    physics.clampToRoom = true;

    // Health
    player.addComponent<HealthComponent>(3, 0.5f);

    // Hurtbox
    player.addComponent<HurtboxComponent>(sf::Vector2f{32.f, 32.f});

    // Hitbox (attack)
    auto& hitbox = player.addComponent<HitboxComponent>();
    hitbox.size = {40.f, 20.f};
    hitbox.damage = 1;
    hitbox.faction = Faction::Player;
    hitbox.active = false;

    // Player control
    player.addComponent<PlayerControlComponent>();

    return player;
}

enum class EnemyType { Slime, Bat };

inline Entity& createEnemy(EntityManager& manager, EnemyType type, sf::Vector2f position, const sf::FloatRect& roomBounds) {
    Entity& enemy = manager.createEntity();
    enemy.position = position;

    // Sprite
    auto& sprite = enemy.addComponent<SpriteComponent>();
    sprite.origin = {14.f, 14.f};

    // Physics
    auto& physics = enemy.addComponent<PhysicsComponent>();
    physics.roomBounds = roomBounds;
    physics.clampToRoom = true;

    // Health
    enemy.addComponent<HealthComponent>(1, 0.f);

    // Hurtbox
    enemy.addComponent<HurtboxComponent>(sf::Vector2f{28.f, 28.f});

    // AI
    auto& ai = enemy.addComponent<AIComponent>();

    // Enemy tag
    enemy.addComponent<EnemyTag>();

    // Configure based on type
    switch (type) {
        case EnemyType::Slime:
            sprite.size = {28.f, 28.f};
            sprite.color = sf::Color(180, 50, 50);
            ai.behavior = AIBehavior::Wander;
            ai.wanderSpeed = 40.f;
            ai.chaseSpeed = 80.f;
            ai.detectionRadius = 150.f;
            ai.loseRadius = 200.f;
            ai.directionChangeInterval = 1.f;
            break;

        case EnemyType::Bat:
            sprite.size = {24.f, 24.f};
            sprite.origin = {12.f, 12.f};
            sprite.color = sf::Color(100, 50, 150);
            ai.behavior = AIBehavior::Erratic;
            ai.wanderSpeed = 60.f;
            ai.chaseSpeed = 100.f;
            ai.detectionRadius = 120.f;
            ai.loseRadius = 180.f;
            ai.directionChangeInterval = 0.3f;
            break;
    }

    return enemy;
}

inline Entity& createHealthPickup(EntityManager& manager, sf::Vector2f position) {
    Entity& pickup = manager.createEntity();
    pickup.position = position;

    // Sprite
    auto& sprite = pickup.addComponent<SpriteComponent>();
    sprite.size = {16.f, 16.f};
    sprite.origin = {8.f, 8.f};
    sprite.color = sf::Color::Red;

    // Hurtbox for collision detection
    pickup.addComponent<HurtboxComponent>(sf::Vector2f{16.f, 16.f});

    // Pickup component
    pickup.addComponent<PickupComponent>(PickupType::Health, 1);

    // Tag
    pickup.addComponent<PickupTag>();

    return pickup;
}

} // namespace EntityFactory
