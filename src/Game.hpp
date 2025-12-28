#pragma once

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "CollisionSystem.hpp"
#include <vector>

enum class GameState {
    Playing,
    Win,
    GameOver
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void reset();
    void checkWinLose();
    void spawnEnemies();

    sf::RenderWindow window;
    GameState state;
    Player player;
    std::vector<Enemy> enemies;
    CollisionSystem collisionSystem;

    // Room bounds
    sf::FloatRect roomBounds;

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr int ENEMY_COUNT = 4;
};
