#pragma once

#include "Player.hpp"
#include "Enemy.hpp"
#include <vector>

class CollisionSystem {
public:
    void update(Player& player, std::vector<Enemy>& enemies);

private:
    bool checkCollision(const sf::FloatRect& a, const sf::FloatRect& b);
};
