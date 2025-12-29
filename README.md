# Dungeon Crawler

A roguelike dungeon crawler built with C++17 and SFML 3.x featuring procedural generation and an Entity Component System (ECS) architecture.

## Features

- Procedurally generated multi-floor dungeons (4-7 rooms per floor)
- Two enemy types with AI: Slimes (wander/chase) and Bats (erratic movement)
- Health system with pickups and invincibility frames
- Pixel art styled menus with mouse hover effects
- Multiple game states: main menu, pause, game over, victory
- Comprehensive test suite (91 tests, 265 assertions)

## Requirements

- CMake 3.16+
- SFML 3.x

```bash
# macOS
brew install sfml
```

## Build & Run

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run
./build/DungeonCrawler
```

## Controls

| Action | Key |
|--------|-----|
| Move | WASD / Arrow Keys |
| Attack | Spacebar |
| Pause | P |
| Restart | R (on game over) |
| Quit | Escape |
| Menu | Mouse click |

## Architecture

The game uses an ECS-lite architecture:

- **Components**: SpriteComponent, PhysicsComponent, HealthComponent, AIComponent, etc.
- **Systems**: PhysicsSystem, AISystem, CollisionSystem, RenderSystem, PickupSystem
- **Core**: AssetManager (singleton), EventBus (pub/sub), StateManager (stack-based states)
- **Game**: Room and Floor classes for procedural dungeon generation

## Testing

```bash
# Build and run tests
cmake --build build
./build/DungeonCrawlerTests
```

## License

MIT License
