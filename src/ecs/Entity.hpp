#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <SFML/Graphics.hpp>

class Component;

class Entity {
public:
    explicit Entity(unsigned int id) : id(id) {}

    unsigned int getId() const { return id; }

    sf::Vector2f position{0.f, 0.f};
    bool active = true;

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        components[std::type_index(typeid(T))] = std::move(component);
        return ref;
    }

    template<typename T>
    T* getComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    const T* getComponent() const {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return static_cast<const T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template<typename T>
    void removeComponent() {
        components.erase(std::type_index(typeid(T)));
    }

private:
    unsigned int id;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};
