# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added
- Clickable text menus with pixel art style font on all game screens (main menu, pause, game over, victory)
- Mouse hover effects on menu buttons (color change with ">" cursor indicator)

### Fixed
- Doors are now walkable - previously player couldn't pass through green doorways
- Game no longer crashes when player is hit by an enemy
- Enemies now properly die when their health reaches zero
- RESUME button in pause menu now continues the game instead of restarting it
- QUIT button in pause/game over/victory screens now correctly returns to main menu

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
