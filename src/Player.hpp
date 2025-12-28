#pragma once

#include <SFML/Graphics.hpp>

class Player {
public:
    Player();

    void handleInput();
    void update(float dt);
    void render(sf::RenderWindow& window) const;

    void takeDamage(int amount);
    void applyKnockback(const sf::Vector2f& fromPos);
    void setRoomBounds(const sf::FloatRect& bounds);
    void reset();

    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    sf::FloatRect getAttackHitbox() const;
    bool isAttacking() const { return attacking; }
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }

private:
    void updateAttack(float dt);
    void clampToRoom();

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f facing;
    sf::FloatRect roomBounds;

    int health;
    float invincibilityTimer;

    bool attacking;
    float attackTimer;
    float attackCooldown;

    static constexpr float SIZE = 32.f;
    static constexpr float SPEED = 120.f;
    static constexpr float ATTACK_DURATION = 0.15f;
    static constexpr float ATTACK_COOLDOWN = 0.3f;
    static constexpr float INVINCIBILITY_DURATION = 0.5f;
    static constexpr int MAX_HEALTH = 3;
};
