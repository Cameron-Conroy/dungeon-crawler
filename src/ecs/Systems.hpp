#pragma once

#include "EntityManager.hpp"
#include "Component.hpp"
#include "../core/EventBus.hpp"
#include "../util/Random.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

// Physics System - handles movement and room clamping
class PhysicsSystem {
public:
    void update(EntityManager& entities, float dt) {
        entities.forEachWith<PhysicsComponent>([dt](Entity& entity) {
            auto* physics = entity.getComponent<PhysicsComponent>();

            entity.position += physics->velocity * dt;

            // Apply friction
            if (physics->friction > 0.f) {
                physics->velocity *= (1.f - physics->friction * dt);
            }

            // Clamp to room bounds
            if (physics->clampToRoom) {
                clampToRoom(entity.position, physics->roomBounds, 16.f);
            }
        });
    }

private:
    static void clampToRoom(sf::Vector2f& pos, const sf::FloatRect& bounds, float halfSize) {
        if (pos.x - halfSize < bounds.position.x)
            pos.x = bounds.position.x + halfSize;
        if (pos.x + halfSize > bounds.position.x + bounds.size.x)
            pos.x = bounds.position.x + bounds.size.x - halfSize;
        if (pos.y - halfSize < bounds.position.y)
            pos.y = bounds.position.y + halfSize;
        if (pos.y + halfSize > bounds.position.y + bounds.size.y)
            pos.y = bounds.position.y + bounds.size.y - halfSize;
    }
};

// AI System - handles enemy behavior
class AISystem {
public:
    void update(EntityManager& entities, float dt, sf::Vector2f playerPos) {
        entities.forEachWith<AIComponent, PhysicsComponent>([this, dt, playerPos](Entity& entity) {
            auto* ai = entity.getComponent<AIComponent>();
            auto* physics = entity.getComponent<PhysicsComponent>();

            float dx = playerPos.x - entity.position.x;
            float dy = playerPos.y - entity.position.y;
            float distToPlayer = std::sqrt(dx * dx + dy * dy);

            // State transitions
            if (distToPlayer < ai->detectionRadius) {
                ai->isChasing = true;
            } else if (distToPlayer > ai->loseRadius) {
                ai->isChasing = false;
            }

            // Movement based on behavior
            if (ai->isChasing) {
                updateChase(physics, ai, entity.position, playerPos);
            } else {
                updateWander(physics, ai, dt);
            }
        });
    }

private:
    void updateWander(PhysicsComponent* physics, AIComponent* ai, float dt) {
        ai->wanderTimer -= dt;
        if (ai->wanderTimer <= 0.f) {
            float interval = ai->behavior == AIBehavior::Erratic ? 0.3f : ai->directionChangeInterval;
            float angle = util::randomFloat(0.f, 2.f * 3.14159f);
            physics->velocity.x = std::cos(angle) * ai->wanderSpeed;
            physics->velocity.y = std::sin(angle) * ai->wanderSpeed;
            ai->wanderTimer = interval + util::randomFloat(0.f, interval);
        }
    }

    void updateChase(PhysicsComponent* physics, AIComponent* ai, sf::Vector2f entityPos, sf::Vector2f playerPos) {
        sf::Vector2f dir = playerPos - entityPos;
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length > 0.f) {
            dir.x /= length;
            dir.y /= length;
            physics->velocity = dir * ai->chaseSpeed;
        }
    }
};

// Player Control System - handles input
class PlayerControlSystem {
public:
    void update(EntityManager& entities, float dt) {
        entities.forEachWith<PlayerControlComponent, PhysicsComponent>([dt](Entity& entity) {
            auto* control = entity.getComponent<PlayerControlComponent>();
            auto* physics = entity.getComponent<PhysicsComponent>();
            auto* hitbox = entity.getComponent<HitboxComponent>();

            // Movement input
            sf::Vector2f input{0.f, 0.f};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                input.y -= 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                input.y += 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                input.x -= 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                input.x += 1.f;
            }

            // Normalize and apply
            float length = std::sqrt(input.x * input.x + input.y * input.y);
            if (length > 0.f) {
                input.x /= length;
                input.y /= length;
                physics->velocity = input * physics->speed;
                control->facing = input;
                if (hitbox) hitbox->facing = input;
            } else {
                physics->velocity = {0.f, 0.f};
            }

            // Attack input
            control->cooldownTimer -= dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && control->cooldownTimer <= 0.f) {
                control->isAttacking = true;
                control->attackTimer = control->attackDuration;
                control->cooldownTimer = control->attackCooldown;
                if (hitbox) hitbox->active = true;
            }

            // Update attack timer
            if (control->isAttacking) {
                control->attackTimer -= dt;
                if (control->attackTimer <= 0.f) {
                    control->isAttacking = false;
                    if (hitbox) hitbox->active = false;
                }
            }
        });
    }
};

// Collision System - handles hitbox/hurtbox collisions
class CollisionSystem {
public:
    void update(EntityManager& entities) {
        // Collect entities with hitboxes and hurtboxes
        std::vector<Entity*> attackers;
        std::vector<Entity*> targets;

        entities.forEachWith<HitboxComponent>([&attackers](Entity& e) {
            auto* hitbox = e.getComponent<HitboxComponent>();
            if (hitbox->active) {
                attackers.push_back(&e);
            }
        });

        entities.forEachWith<HurtboxComponent, HealthComponent>([&targets](Entity& e) {
            targets.push_back(&e);
        });

        // Check collisions
        for (auto* attacker : attackers) {
            auto* hitbox = attacker->getComponent<HitboxComponent>();
            auto hitBounds = hitbox->getBounds(attacker->position);

            for (auto* target : targets) {
                if (attacker == target) continue;

                auto* health = target->getComponent<HealthComponent>();
                if (!health->isAlive() || health->isInvincible()) continue;

                // Check faction (don't hit same faction)
                auto* targetHitbox = target->getComponent<HitboxComponent>();
                if (targetHitbox && targetHitbox->faction == hitbox->faction) continue;

                auto* hurtbox = target->getComponent<HurtboxComponent>();
                auto hurtBounds = hurtbox->getBounds(target->position);

                if (hitBounds.findIntersection(hurtBounds)) {
                    health->takeDamage(hitbox->damage);

                    // Emit events
                    if (!health->isAlive()) {
                        if (target->hasComponent<EnemyTag>()) {
                            target->active = false;  // Mark for removal
                            EventBus::instance().emit<EnemyDiedEvent>(
                                target->getId(), target->position.x, target->position.y
                            );
                        } else if (target->hasComponent<PlayerControlComponent>()) {
                            EventBus::instance().emit<PlayerDiedEvent>();
                        }
                    } else if (target->hasComponent<PlayerControlComponent>()) {
                        EventBus::instance().emit<PlayerDamagedEvent>(
                            hitbox->damage, attacker->getId()
                        );
                    }
                }
            }
        }

        // Enemy contact damage to player
        entities.forEachWith<EnemyTag, HurtboxComponent>([&entities](Entity& enemy) {
            auto* enemyHurtbox = enemy.getComponent<HurtboxComponent>();
            auto enemyBounds = enemyHurtbox->getBounds(enemy.position);

            entities.forEachWith<PlayerControlComponent, HurtboxComponent, HealthComponent>(
                [&enemy, &enemyBounds](Entity& player) {
                    auto* playerHurtbox = player.getComponent<HurtboxComponent>();
                    auto* playerHealth = player.getComponent<HealthComponent>();

                    if (!playerHealth->isAlive() || playerHealth->isInvincible()) return;

                    auto playerBounds = playerHurtbox->getBounds(player.position);
                    if (enemyBounds.findIntersection(playerBounds)) {
                        playerHealth->takeDamage(1);

                        // Apply knockback
                        sf::Vector2f dir = player.position - enemy.position;
                        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                        if (length > 0.f) {
                            dir.x /= length;
                            dir.y /= length;
                            player.position += dir * 20.f;
                        }

                        if (!playerHealth->isAlive()) {
                            EventBus::instance().emit<PlayerDiedEvent>();
                        } else {
                            EventBus::instance().emit<PlayerDamagedEvent>(1, enemy.getId());
                        }
                    }
                }
            );
        });
    }
};

// Pickup System - handles pickup collection
class PickupSystem {
public:
    void update(EntityManager& entities) {
        Entity* player = nullptr;
        entities.forEachWith<PlayerControlComponent>([&player](Entity& e) {
            player = &e;
        });

        if (!player) return;

        auto* playerHurtbox = player->getComponent<HurtboxComponent>();
        if (!playerHurtbox) return;
        auto playerBounds = playerHurtbox->getBounds(player->position);

        entities.forEachWith<PickupComponent, HurtboxComponent>([&](Entity& pickup) {
            auto* pickupComp = pickup.getComponent<PickupComponent>();
            if (pickupComp->collected) return;

            auto* hurtbox = pickup.getComponent<HurtboxComponent>();
            auto pickupBounds = hurtbox->getBounds(pickup.position);

            if (playerBounds.findIntersection(pickupBounds)) {
                pickupComp->collected = true;
                pickup.active = false;

                // Apply effect
                if (pickupComp->type == PickupType::Health) {
                    auto* health = player->getComponent<HealthComponent>();
                    if (health) {
                        health->current = std::min(health->current + pickupComp->value, health->max);
                    }
                }

                EventBus::instance().emit<PickupCollectedEvent>(
                    pickup.getId(),
                    static_cast<int>(pickupComp->type),
                    pickupComp->value
                );
            }
        });
    }
};

// Render System - draws all entities
class RenderSystem {
public:
    void render(EntityManager& entities, sf::RenderWindow& window) {
        entities.forEachWith<SpriteComponent>([&window](Entity& entity) {
            auto* sprite = entity.getComponent<SpriteComponent>();

            sf::RectangleShape shape(sprite->size);
            shape.setOrigin(sprite->origin);
            shape.setPosition(entity.position);
            shape.setFillColor(sprite->color);

            // Blink during invincibility
            auto* health = entity.getComponent<HealthComponent>();
            if (health && health->isInvincible()) {
                if (static_cast<int>(health->invincibilityTimer * 10) % 2 == 0) {
                    auto color = sprite->color;
                    color.a = 128;
                    shape.setFillColor(color);
                }
            }

            window.draw(shape);

            // Draw attack hitbox if active
            auto* control = entity.getComponent<PlayerControlComponent>();
            auto* hitbox = entity.getComponent<HitboxComponent>();
            if (control && hitbox && hitbox->active) {
                sf::RectangleShape attackShape(hitbox->size);
                attackShape.setOrigin({hitbox->size.x / 2.f, hitbox->size.y / 2.f});
                sf::Vector2f attackPos = entity.position + hitbox->facing * (16.f + hitbox->size.x / 2.f);
                attackShape.setPosition(attackPos);

                float angle = std::atan2(hitbox->facing.y, hitbox->facing.x) * 180.f / 3.14159f;
                attackShape.setRotation(sf::degrees(angle));

                attackShape.setFillColor(sf::Color(255, 255, 0, 150));
                window.draw(attackShape);
            }
        });
    }
};

// Health System - updates invincibility timers
class HealthSystem {
public:
    void update(EntityManager& entities, float dt) {
        entities.forEachWith<HealthComponent>([dt](Entity& entity) {
            auto* health = entity.getComponent<HealthComponent>();
            health->update(dt);
        });
    }
};
