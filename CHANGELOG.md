# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

## [0.1.0] - 2025-12-28

### Added
- Player movement with WASD or arrow keys (8 directions, normalized diagonal speed)
- Melee attack with spacebar - short-range sword swing in facing direction
- 4 enemies spawn in the room with wander/chase AI behavior
- Enemies chase player when within 150px, return to wandering when player escapes beyond 200px
- Health system with 3 HP, displayed as red squares in top-left corner
- Invincibility frames (0.5s) after taking damage with visual blinking feedback
- Knockback when hit by enemies
- Win condition: kill all enemies (screen darkens)
- Game over condition: lose all health (screen darkens)
- Press R to restart after win or game over
- Press Escape to quit
