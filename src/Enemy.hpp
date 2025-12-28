#pragma once

#include <SFML/Graphics.hpp>

enum class EnemyState {
    Wander,
    Chase
};

class Enemy {
public:
    Enemy(const sf::Vector2f& startPos);

    void update(float dt, const sf::Vector2f& playerPos);
    void render(sf::RenderWindow& window) const;

    void takeDamage(int amount);
    void setRoomBounds(const sf::FloatRect& bounds);

    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    bool isAlive() const { return health > 0; }

private:
    void updateWander(float dt);
    void updateChase(const sf::Vector2f& playerPos);
    void clampToRoom();

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::FloatRect roomBounds;

    EnemyState state;
    int health;
    float wanderTimer;

    static constexpr float SIZE = 28.f;
    static constexpr float WANDER_SPEED = 40.f;
    static constexpr float CHASE_SPEED = 80.f;
    static constexpr float DETECTION_RADIUS = 150.f;
    static constexpr float LOSE_RADIUS = 200.f;
    static constexpr int MAX_HEALTH = 1;
};
