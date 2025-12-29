# Pixel Art Menu System Design

## Overview

Add clickable text menus with a retro pixel art aesthetic to all game screens.

## Decisions

- **Font**: "Press Start 2P" TTF pixel font (OFL license, bundled in assets/)
- **Scope**: Main Menu, Pause, Game Over, Victory screens
- **Interaction**: Color change (gray → yellow) + animated ">" cursor indicator
- **In-game HUD**: Unchanged (hearts and bars remain)

## File Structure

```
assets/
  fonts/
    PressStart2P-Regular.ttf

src/
  ui/
    MenuButton.hpp    # New reusable button component
```

## Font Sizes

- Menu titles: 24px
- Menu options: 16px
- Small labels/stats: 8px

## MenuButton Component

```cpp
class MenuButton {
    sf::Text label;
    sf::Text cursor;          // ">" indicator
    bool hovered = false;

    sf::Color normalColor{150, 150, 150};
    sf::Color hoverColor{255, 255, 100};

    void update(sf::Vector2f mousePos);
    void render(sf::RenderWindow& window);
    bool isClicked(sf::Vector2f mousePos, bool mousePressed);
    void setPosition(float x, float y);
};
```

Cursor animates with subtle horizontal bounce:
```cpp
float cursorOffset = sin(time * 6.f) * 3.f;
```

## Screen Layouts

### Main Menu
```
      DUNGEON
      CRAWLER

    > START GAME
      QUIT
```

### Pause Menu
```
     PAUSED

   > RESUME
     RESTART
     QUIT
```

### Game Over
```
    GAME OVER

    Floor: 2
    Enemies: 12
    Rooms: 5

   > RETRY
     QUIT
```

### Victory
```
    VICTORY!

    Floors: 3
    Enemies: 28
    Rooms: 14

   > PLAY AGAIN
     QUIT
```

## Color Scheme

- Titles: White (or thematic - red for Game Over, gold for Victory)
- Stats: Gray (#999999)
- Buttons normal: Gray (#999999)
- Buttons hovered: Yellow (#FFFF66)

## Modified Files

- `AssetManager.hpp` - add font loading
- `Application.hpp` - load font at startup
- `MainMenuState.cpp/hpp`
- `PausedState.cpp/hpp`
- `GameOverState.cpp/hpp`
- `VictoryState.cpp/hpp`

## Implementation Notes

- Use `sf::Text::getGlobalBounds()` for click detection
- Use `sf::Mouse::getPosition(window)` for mouse tracking
- Existing game logic unchanged
