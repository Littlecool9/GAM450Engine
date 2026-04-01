/**
 * @file ParticleContainer.h
 * @author josh.siegel
 * @brief // This class acts as a container to hold and manage particles.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    struct Particle
    {
        // Transform variables
        glm::vec2 position{0.0f, 0.0f};
        glm::vec2 scale{0.0f, 0.0f};
        float rotation{0.0f};

        // Physics variables
        glm::vec2 velocity{0.0f, 0.0f};
        glm::vec2 acceleration{0.0f, 0.0f};
        float rotationSpeed{0.0f};

        // Particle specific variables
        float lifetime{0.0f};
        glm::vec2 uvOffset{0.0f, 0.0f};
    };


    class ParticleContainer
    {
       public:
        ParticleContainer(unsigned _maxParticles, bool _oneShot);

        void Update(std::function<void(Particle& particle, float dt)> const& lambda, float dt);

        // @brief Render the Object each frame.
        void Render(std::function<void(const Particle& particle)> const& lambda) const;

        bool IsEmpty() const;

        bool IsFull() const;

        Particle* AllocateParticle();

        void KillParticle(Particle& particle);

       private:
        std::vector<Particle> mParticles;

        unsigned mNumActiveParticles{0};
        unsigned mNumFreeParticles{0};

        bool mOneShot{false};
    };

}  // namespace mts