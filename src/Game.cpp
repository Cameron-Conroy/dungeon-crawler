#include "Game.hpp"
#include <cstdlib>

Game::Game()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Dungeon Crawler")
    , state(GameState::Playing)
    , roomBounds({40.f, 40.f}, {WINDOW_WIDTH - 80.f, WINDOW_HEIGHT - 80.f})
{
    window.setFramerateLimit(60);
    player.setRoomBounds(roomBounds);
    spawnEnemies();
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
            if (keyPressed->code == sf::Keyboard::Key::R &&
                (state == GameState::Win || state == GameState::GameOver)) {
                reset();
            }
        }
    }
}

void Game::update(float dt) {
    switch (state) {
        case GameState::Playing:
            player.handleInput();
            player.update(dt);

            for (auto& enemy : enemies) {
                enemy.update(dt, player.getPosition());
            }

            collisionSystem.update(player, enemies);
            checkWinLose();
            break;

        case GameState::Win:
        case GameState::GameOver:
            // Wait for R key in processEvents
            break;
    }
}

void Game::render() {
    window.clear(sf::Color(40, 40, 50));

    // Draw room border
    sf::RectangleShape roomShape({roomBounds.size.x, roomBounds.size.y});
    roomShape.setPosition(roomBounds.position);
    roomShape.setFillColor(sf::Color(60, 60, 70));
    roomShape.setOutlineColor(sf::Color(100, 100, 120));
    roomShape.setOutlineThickness(4.f);
    window.draw(roomShape);

    // Draw enemies
    for (const auto& enemy : enemies) {
        enemy.render(window);
    }

    // Draw player
    player.render(window);

    // Draw UI (health)
    for (int i = 0; i < player.getHealth(); ++i) {
        sf::RectangleShape heart({20.f, 20.f});
        heart.setPosition({10.f + i * 25.f, 10.f});
        heart.setFillColor(sf::Color::Red);
        window.draw(heart);
    }

    // Draw state overlay
    if (state == GameState::Win || state == GameState::GameOver) {
        sf::RectangleShape overlay({static_cast<float>(WINDOW_WIDTH),
                                    static_cast<float>(WINDOW_HEIGHT)});
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        // Note: For text, you'd need to load a font
        // For now the overlay indicates game end - press R to restart
    }

    window.display();
}

void Game::reset() {
    player.reset();
    player.setRoomBounds(roomBounds);
    enemies.clear();
    spawnEnemies();
    state = GameState::Playing;
}

void Game::checkWinLose() {
    if (!player.isAlive()) {
        state = GameState::GameOver;
        return;
    }

    bool allDead = true;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive()) {
            allDead = false;
            break;
        }
    }
    if (allDead) {
        state = GameState::Win;
    }
}

void Game::spawnEnemies() {
    for (int i = 0; i < ENEMY_COUNT; ++i) {
        float x = roomBounds.position.x + 100.f + (std::rand() % static_cast<int>(roomBounds.size.x - 200.f));
        float y = roomBounds.position.y + 100.f + (std::rand() % static_cast<int>(roomBounds.size.y - 200.f));
        Enemy enemy({x, y});
        enemy.setRoomBounds(roomBounds);
        enemies.push_back(enemy);
    }
}
