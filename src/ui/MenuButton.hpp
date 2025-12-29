#pragma once

#include "../core/AssetManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <optional>

class MenuButton {
public:
    MenuButton(const std::string& text, unsigned int fontSize = 16)
        : fontSize(fontSize), labelText(text)
    {
        if (AssetManager::instance().hasFont("pixel")) {
            auto& font = AssetManager::instance().getFont("pixel");
            label.emplace(font, text, fontSize);
            label->setFillColor(normalColor);

            cursor.emplace(font, ">", fontSize);
            cursor->setFillColor(hoverColor);
        }
    }

    void setPosition(float x, float y) {
        if (label) {
            label->setPosition({x, y});
        }
        cursorBaseX = x - fontSize * 1.5f;
        cursorY = y;
    }

    void update(sf::Vector2f mousePos, float dt) {
        time += dt;

        if (!label) return;

        // Check if mouse is over the label
        sf::FloatRect bounds = label->getGlobalBounds();
        hovered = bounds.contains(mousePos);

        // Update colors
        label->setFillColor(hovered ? hoverColor : normalColor);

        // Update cursor position with bounce animation
        if (hovered && cursor) {
            float bounce = std::sin(time * 6.f) * 3.f;
            cursor->setPosition({cursorBaseX + bounce, cursorY});
        }
    }

    void render(sf::RenderWindow& window) {
        if (label) {
            window.draw(*label);
        }
        if (hovered && cursor) {
            window.draw(*cursor);
        }
    }

    bool isClicked(bool mousePressed) const {
        return hovered && mousePressed;
    }

    bool isHovered() const {
        return hovered;
    }

    sf::FloatRect getBounds() const {
        if (label) {
            return label->getGlobalBounds();
        }
        return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    }

private:
    std::optional<sf::Text> label;
    std::optional<sf::Text> cursor;
    std::string labelText;
    unsigned int fontSize;

    bool hovered = false;
    float time = 0.f;
    float cursorBaseX = 0.f;
    float cursorY = 0.f;

    sf::Color normalColor{150, 150, 150};
    sf::Color hoverColor{255, 255, 100};
};
