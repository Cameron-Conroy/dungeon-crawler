#include "Player.hpp"
#include <cmath>

Player::Player()
    : position(400.f, 300.f)
    , velocity(0.f, 0.f)
    , facing(1.f, 0.f)
    , health(MAX_HEALTH)
    , invincibilityTimer(0.f)
    , attacking(false)
    , attackTimer(0.f)
    , attackCooldown(0.f)
{
}

void Player::handleInput() {
    velocity = {0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        velocity.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        velocity.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        velocity.x -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        velocity.x += 1.f;
    }

    // Normalize diagonal movement
    float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (length > 0.f) {
        velocity.x /= length;
        velocity.y /= length;
        velocity.x *= SPEED;
        velocity.y *= SPEED;
        facing = {velocity.x / SPEED, velocity.y / SPEED};
    }
}

void Player::update(float dt) {
    position += velocity * dt;
    clampToRoom();
    updateAttack(dt);

    if (invincibilityTimer > 0.f) {
        invincibilityTimer -= dt;
    }
}

void Player::updateAttack(float dt) {
    attackCooldown -= dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && attackCooldown <= 0.f) {
        attacking = true;
        attackTimer = ATTACK_DURATION;
        attackCooldown = ATTACK_COOLDOWN;
    }

    if (attacking) {
        attackTimer -= dt;
        if (attackTimer <= 0.f) {
            attacking = false;
        }
    }
}

void Player::render(sf::RenderWindow& window) const {
    // Draw player
    sf::RectangleShape shape({SIZE, SIZE});
    shape.setOrigin({SIZE / 2.f, SIZE / 2.f});
    shape.setPosition(position);

    // Blink during invincibility
    if (invincibilityTimer > 0.f && static_cast<int>(invincibilityTimer * 10) % 2 == 0) {
        shape.setFillColor(sf::Color(100, 255, 100, 128));
    } else {
        shape.setFillColor(sf::Color::Green);
    }
    window.draw(shape);

    // Draw attack hitbox
    if (attacking) {
        sf::RectangleShape attackShape({40.f, 20.f});
        attackShape.setOrigin({0.f, 10.f});
        attackShape.setPosition(position + facing * (SIZE / 2.f + 5.f));

        // Rotate to face direction
        float angle = std::atan2(facing.y, facing.x) * 180.f / 3.14159f;
        attackShape.setRotation(sf::degrees(angle));

        attackShape.setFillColor(sf::Color(255, 255, 0, 150));
        window.draw(attackShape);
    }
}

void Player::takeDamage(int amount) {
    if (invincibilityTimer > 0.f) return;
    health -= amount;
    invincibilityTimer = INVINCIBILITY_DURATION;
}

void Player::applyKnockback(const sf::Vector2f& fromPos) {
    sf::Vector2f dir = position - fromPos;
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length > 0.f) {
        dir.x /= length;
        dir.y /= length;
        position += dir * 20.f;  // Knockback distance
        clampToRoom();
    }
}

void Player::setRoomBounds(const sf::FloatRect& bounds) {
    roomBounds = bounds;
}

void Player::reset() {
    position = {400.f, 300.f};
    velocity = {0.f, 0.f};
    facing = {1.f, 0.f};
    health = MAX_HEALTH;
    invincibilityTimer = 0.f;
    attacking = false;
    attackTimer = 0.f;
    attackCooldown = 0.f;
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect({position.x - SIZE / 2.f, position.y - SIZE / 2.f}, {SIZE, SIZE});
}

sf::FloatRect Player::getAttackHitbox() const {
    sf::Vector2f attackPos = position + facing * (SIZE / 2.f + 20.f);
    return sf::FloatRect({attackPos.x - 20.f, attackPos.y - 10.f}, {40.f, 20.f});
}

void Player::clampToRoom() {
    float halfSize = SIZE / 2.f;
    if (position.x - halfSize < roomBounds.position.x)
        position.x = roomBounds.position.x + halfSize;
    if (position.x + halfSize > roomBounds.position.x + roomBounds.size.x)
        position.x = roomBounds.position.x + roomBounds.size.x - halfSize;
    if (position.y - halfSize < roomBounds.position.y)
        position.y = roomBounds.position.y + halfSize;
    if (position.y + halfSize > roomBounds.position.y + roomBounds.size.y)
        position.y = roomBounds.position.y + roomBounds.size.y - halfSize;
}
