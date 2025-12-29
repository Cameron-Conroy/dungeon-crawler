#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

class AssetManager {
public:
    static AssetManager& instance() {
        static AssetManager inst;
        return inst;
    }

    // Textures
    sf::Texture& getTexture(const std::string& id) {
        auto it = textures.find(id);
        if (it != textures.end()) {
            return *it->second;
        }
        return *placeholderTexture;
    }

    bool loadTexture(const std::string& id, const std::string& path) {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(path)) {
            std::cerr << "[AssetManager] Failed to load texture: " << path << "\n";
            return false;
        }
        textures[id] = std::move(texture);
        return true;
    }

    bool hasTexture(const std::string& id) const {
        return textures.find(id) != textures.end();
    }

    // Fonts
    sf::Font& getFont(const std::string& id) {
        auto it = fonts.find(id);
        if (it != fonts.end()) {
            return *it->second;
        }
        if (!defaultFont) {
            throw std::runtime_error("No default font loaded and font '" + id + "' not found");
        }
        return *defaultFont;
    }

    bool loadFont(const std::string& id, const std::string& path) {
        auto font = std::make_unique<sf::Font>();
        if (!font->openFromFile(path)) {
            std::cerr << "[AssetManager] Failed to load font: " << path << "\n";
            return false;
        }
        fonts[id] = std::move(font);
        if (!defaultFont) {
            defaultFont = fonts[id].get();
        }
        return true;
    }

    bool hasFont(const std::string& id) const {
        return fonts.find(id) != fonts.end();
    }

    // Sound Buffers
    sf::SoundBuffer& getSoundBuffer(const std::string& id) {
        auto it = soundBuffers.find(id);
        if (it != soundBuffers.end()) {
            return *it->second;
        }
        throw std::runtime_error("Sound buffer not found: " + id);
    }

    bool loadSoundBuffer(const std::string& id, const std::string& path) {
        auto buffer = std::make_unique<sf::SoundBuffer>();
        if (!buffer->loadFromFile(path)) {
            std::cerr << "[AssetManager] Failed to load sound: " << path << "\n";
            return false;
        }
        soundBuffers[id] = std::move(buffer);
        return true;
    }

    bool hasSoundBuffer(const std::string& id) const {
        return soundBuffers.find(id) != soundBuffers.end();
    }

    // Clear all loaded assets
    void clear() {
        textures.clear();
        fonts.clear();
        soundBuffers.clear();
        defaultFont = nullptr;
    }

private:
    AssetManager() {
        createPlaceholderTexture();
    }

    void createPlaceholderTexture() {
        placeholderTexture = std::make_unique<sf::Texture>();
        sf::Image img({32, 32}, sf::Color::Magenta);
        // Checkerboard pattern
        for (unsigned y = 0; y < 32; ++y) {
            for (unsigned x = 0; x < 32; ++x) {
                if ((x / 8 + y / 8) % 2 == 0) {
                    img.setPixel({x, y}, sf::Color::Black);
                }
            }
        }
        if (!placeholderTexture->loadFromImage(img)) {
            std::cerr << "[AssetManager] Failed to create placeholder texture\n";
        }
    }

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::unique_ptr<sf::Texture> placeholderTexture;
    sf::Font* defaultFont = nullptr;

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
};
