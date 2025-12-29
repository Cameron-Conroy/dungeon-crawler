#pragma once

#include "Entity.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class EntityManager {
public:
    Entity& createEntity() {
        auto entity = std::make_unique<Entity>(nextId++);
        Entity& ref = *entity;
        entities.push_back(std::move(entity));
        return ref;
    }

    void destroyEntity(unsigned int id) {
        auto it = std::find_if(entities.begin(), entities.end(),
            [id](const auto& e) { return e->getId() == id; });
        if (it != entities.end()) {
            (*it)->active = false;
        }
    }

    void cleanup() {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const auto& e) { return !e->active; }),
            entities.end()
        );
    }

    void clear() {
        entities.clear();
    }

    Entity* getEntity(unsigned int id) {
        auto it = std::find_if(entities.begin(), entities.end(),
            [id](const auto& e) { return e->getId() == id; });
        return it != entities.end() ? it->get() : nullptr;
    }

    template<typename Func>
    void forEach(Func&& func) {
        for (auto& entity : entities) {
            if (entity->active) {
                func(*entity);
            }
        }
    }

    template<typename T, typename Func>
    void forEachWith(Func&& func) {
        for (auto& entity : entities) {
            if (entity->active && entity->hasComponent<T>()) {
                func(*entity);
            }
        }
    }

    template<typename T1, typename T2, typename Func>
    void forEachWith(Func&& func) {
        for (auto& entity : entities) {
            if (entity->active && entity->hasComponent<T1>() && entity->hasComponent<T2>()) {
                func(*entity);
            }
        }
    }

    template<typename T1, typename T2, typename T3, typename Func>
    void forEachWith(Func&& func) {
        for (auto& entity : entities) {
            if (entity->active && entity->hasComponent<T1>() && entity->hasComponent<T2>() && entity->hasComponent<T3>()) {
                func(*entity);
            }
        }
    }

    size_t count() const { return entities.size(); }

    template<typename T>
    size_t countWith() const {
        size_t result = 0;
        for (const auto& entity : entities) {
            if (entity->active && entity->hasComponent<T>()) {
                ++result;
            }
        }
        return result;
    }

private:
    std::vector<std::unique_ptr<Entity>> entities;
    unsigned int nextId = 1;
};
