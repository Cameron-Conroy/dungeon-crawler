#include "Enemy.hpp"
#include <cmath>
#include <cstdlib>

Enemy::Enemy(const sf::Vector2f& startPos)
    : position(startPos)
    , velocity(0.f, 0.f)
    , state(EnemyState::Wander)
    , health(MAX_HEALTH)
    , wanderTimer(0.f)
{
}

void Enemy::update(float dt, const sf::Vector2f& playerPos) {
    if (!isAlive()) return;

    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float distToPlayer = std::sqrt(dx * dx + dy * dy);

    // State transitions
    if (distToPlayer < DETECTION_RADIUS) {
        state = EnemyState::Chase;
    } else if (distToPlayer > LOSE_RADIUS) {
        state = EnemyState::Wander;
    }

    // Movement based on state
    switch (state) {
        case EnemyState::Wander:
            updateWander(dt);
            break;
        case EnemyState::Chase:
            updateChase(playerPos);
            break;
    }

    position += velocity * dt;
    clampToRoom();
}

void Enemy::updateWander(float dt) {
    wanderTimer -= dt;
    if (wanderTimer <= 0.f) {
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
        velocity.x = std::cos(angle) * WANDER_SPEED;
        velocity.y = std::sin(angle) * WANDER_SPEED;
        wanderTimer = 1.f + static_cast<float>(std::rand()) / RAND_MAX;
    }
}

void Enemy::updateChase(const sf::Vector2f& playerPos) {
    sf::Vector2f dir = playerPos - position;
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length > 0.f) {
        dir.x /= length;
        dir.y /= length;
        velocity = dir * CHASE_SPEED;
    }
}

void Enemy::render(sf::RenderWindow& window) const {
    if (!isAlive()) return;

    sf::RectangleShape shape({SIZE, SIZE});
    shape.setOrigin({SIZE / 2.f, SIZE / 2.f});
    shape.setPosition(position);

    // Color based on state
    if (state == EnemyState::Chase) {
        shape.setFillColor(sf::Color(255, 100, 100));  // Brighter red when chasing
    } else {
        shape.setFillColor(sf::Color(180, 50, 50));    // Darker red when wandering
    }

    window.draw(shape);
}

void Enemy::takeDamage(int amount) {
    health -= amount;
}

void Enemy::setRoomBounds(const sf::FloatRect& bounds) {
    roomBounds = bounds;
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect({position.x - SIZE / 2.f, position.y - SIZE / 2.f}, {SIZE, SIZE});
}

void Enemy::clampToRoom() {
    float halfSize = SIZE / 2.f;
    if (position.x - halfSize < roomBounds.position.x) {
        position.x = roomBounds.position.x + halfSize;
        velocity.x *= -1;
    }
    if (position.x + halfSize > roomBounds.position.x + roomBounds.size.x) {
        position.x = roomBounds.position.x + roomBounds.size.x - halfSize;
        velocity.x *= -1;
    }
    if (position.y - halfSize < roomBounds.position.y) {
        position.y = roomBounds.position.y + halfSize;
        velocity.y *= -1;
    }
    if (position.y + halfSize > roomBounds.position.y + roomBounds.size.y) {
        position.y = roomBounds.position.y + roomBounds.size.y - halfSize;
        velocity.y *= -1;
    }
}
