# Roguelike Architecture Design

## Overview

Transform the single-room dungeon crawler into a roguelike with full architecture supporting procedural floors, entity components, and run-based progression.

## Core Architecture

### Asset Manager

Singleton `AssetManager` that loads and caches textures, fonts, and sounds:

```cpp
AssetManager::load<Texture>("player", "assets/player.png")
AssetManager::get<Texture>("player")
```

- Lazy loading: first `get()` triggers load if not cached
- Supports: `sf::Texture`, `sf::Font`, `sf::SoundBuffer`
- Missing assets return placeholder (magenta square, default font) and log warning
- Optional preload manifest for loading screens

Directory structure:
```
assets/
  textures/
    player.png
    enemies/
    pickups/
  fonts/
  sounds/
    sfx/
    music/
```

### Entity Component System (ECS-lite)

Lightweight approach without external libraries:

**Entity Base**
- `Entity`: id, position, active flag, component container
- Entities are containers; all behavior comes from components and systems

**Core Components**
- `SpriteComponent` — texture ID, size, color tint, animation frame
- `PhysicsComponent` — velocity, friction, room bounds reference
- `HealthComponent` — current HP, max HP, invincibility timer
- `HurtboxComponent` — bounding box for receiving damage
- `HitboxComponent` — bounding box for dealing damage, damage amount, faction (player/enemy)
- `AIComponent` — behavior type enum (Wander, Chase, Patrol), detection radius, target reference
- `PickupComponent` — effect type (heal, speed boost, damage up), collected flag

**Systems**
- `PhysicsSystem` — applies velocity, handles room boundary clamping
- `AISystem` — runs behavior logic based on `AIComponent` type
- `CollisionSystem` — checks hitbox vs hurtbox, emits `DamageDealt` events
- `RenderSystem` — draws all entities with `SpriteComponent`
- `PickupSystem` — detects player overlap with pickups, applies effects, emits `PickupCollected`
- `PlayerControlSystem` — handles input for the player entity

**Migration**
Existing `Player` and `Enemy` classes become factory functions:
- `createPlayer()` returns Entity with Sprite, Physics, Health, Hurtbox, Hitbox components
- `createEnemy(type)` returns Entity with appropriate components for that enemy type

### Event Bus

Central `EventBus` singleton with synchronous pub/sub:

```cpp
EventBus::publish<EnemyDied>(event)
EventBus::subscribe<EnemyDied>(callback)
```

**Core Events**

| Event | Payload | Emitted By | Consumed By |
|-------|---------|------------|-------------|
| `EnemyDied` | enemy ID, position | CollisionSystem | Room (track clear), SpawnSystem (drops) |
| `PlayerDamaged` | amount, source | CollisionSystem | UI (health), CameraSystem (shake) |
| `PlayerDied` | — | HealthSystem | GameState (trigger GameOver) |
| `RoomCleared` | room ID | Room | Doors (unlock), UI (indicator) |
| `PickupCollected` | pickup type, value | PickupSystem | PlayerStats, UI, AudioSystem |
| `RoomEntered` | room ID | Floor | Room (spawn enemies), UI (minimap) |
| `FloorCompleted` | floor number | Floor | GameState (generate next floor) |

### Game State Machine

States as classes inheriting from `GameState`:
- `enter()` — called on transition in
- `exit()` — called on transition out
- `update(dt)` — per-frame logic
- `render(window)` — drawing
- `handleEvent(event)` — SFML events

Stack-based state manager:
```cpp
StateManager::push(state)    // overlay (pause menu)
StateManager::pop()          // return to previous
StateManager::swap(state)    // replace current
```

**Initial States**
- `MainMenuState` — title, "Press Enter to Start"
- `PlayingState` — owns Floor, RunState, all systems
- `PausedState` — overlay, resume/quit options
- `GameOverState` — death screen, restart prompt

## Room & Floor System

### Room Structure

Each room is self-contained:
- `Room` class holds dimensions, enemy spawn points, pickup spawn points, door positions (N/S/E/W)
- Rooms defined as data (JSON or embedded structs), not hardcoded logic
- Room template library: `SmallArena`, `LongCorridor`, `TreasureRoom`, `BossRoom`

### Floor Generation

A floor is a graph of connected rooms:
- `Floor` class generates layout: pick start room, randomly attach 3-6 rooms via doors, place exit in furthest room
- Grid-based placement prevents overlaps (room at [2,1] connects east to [3,1])
- Each floor increases difficulty: more rooms, tougher enemy compositions

### Room Transitions

When player touches a door:
1. `RoomCleared` event must have fired (all enemies dead) or door stays locked
2. Fade-out transition
3. Unload current room entities, load next room entities
4. Position player at matching entrance door
5. Fade-in transition

### Run State

`RunState` object persists across rooms within a single run:
- Player health, collected pickups, current floor number, rooms visited
- Reset entirely on death (permadeath)
- Passed to each room for context

## Playable Prototype Content

### Floor Layout

- Floor 1: 4-5 rooms in random branching layout
- Room types: `StartRoom` (no enemies), `CombatRoom` (2-4 enemies), `ExitRoom` (floor stairs)
- Simple placement: start in center, randomly attach rooms to cardinal directions

### Enemy Roster

Two enemy types to validate ECS:
- `Slime` — current enemy, wander/chase behavior, 1 HP
- `Bat` — faster, erratic movement (frequent random direction changes), 1 HP

Both use same components with different `AIComponent` configs.

### Pickup System

One pickup type:
- `HealthPickup` — restores 1 HP, 30% drop chance when room clears
- Spawns at enemy death position
- Disappears when collected, emits `PickupCollected` event

### Win/Lose Conditions

- **Clear floor**: reach exit room after clearing required rooms → generate floor 2 (harder)
- **Death**: health hits 0 → `GameOverState`, show rooms cleared / floors reached
- **Victory**: clear floor 3 → `VictoryState` (prototype "win")

### Minimal UI

- Health hearts (top-left)
- Current floor number (top-right)
- Minimap showing visited rooms (small squares, highlight current)
