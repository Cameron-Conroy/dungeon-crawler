#include "CollisionSystem.hpp"

void CollisionSystem::update(Player& player, std::vector<Enemy>& enemies) {
    if (!player.isAlive()) return;

    auto playerBounds = player.getBounds();

    for (auto& enemy : enemies) {
        if (!enemy.isAlive()) continue;

        auto enemyBounds = enemy.getBounds();

        // Attack hits enemy
        if (player.isAttacking()) {
            if (checkCollision(player.getAttackHitbox(), enemyBounds)) {
                enemy.takeDamage(1);
            }
        }

        // Enemy touches player
        if (checkCollision(playerBounds, enemyBounds)) {
            player.takeDamage(1);
            player.applyKnockback(enemy.getPosition());
        }
    }
}

bool CollisionSystem::checkCollision(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.findIntersection(b).has_value();
}
