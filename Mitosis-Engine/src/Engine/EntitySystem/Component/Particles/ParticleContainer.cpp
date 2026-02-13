/**
 * @file ParticleContainer.cpp
 * @author josh.siegel
 * @brief // This class acts as a container to hold and manage particles.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ParticleContainer.h"

namespace mts
{
    ParticleContainer::ParticleContainer(unsigned maxParticles, bool oneShot)
        : mParticles(), mNumActiveParticles(0), mNumFreeParticles(maxParticles), mOneShot(oneShot)
    {
        // Fill the particle vector with the max number of particle objects
        mParticles.reserve(maxParticles);
        for (unsigned i = 0; i < maxParticles; ++i)
        {
            mParticles.emplace_back(Particle());
        }
    }

    void ParticleContainer::Update(std::function<void(Particle& particle, float dt)> const &lambda,
                                   float dt)
    {
        // Pass all particles and current dt to lambda update function
        for (auto& particle : mParticles)
        {
            lambda(particle, dt);
        }
    }

    // @brief Render the Object each frame.
    void ParticleContainer::Render(
        std::function<void(const Particle& particle)> const& lambda) const
    {
        // Pass all particles to lambda render function
        for (const auto& particle : mParticles)
        {
            lambda(particle);
        }
    }

    bool ParticleContainer::IsEmpty() const
    {
        return mNumActiveParticles == 0;
    }

    bool ParticleContainer::IsFull() const
    {
        return mNumFreeParticles == 0;
    }

    Particle* ParticleContainer::AllocateParticle()
    {
        if (!IsFull())
        {
            for (Particle& particle : mParticles)
            {
                if (particle.lifetime == 0)
                {
                    ++mNumActiveParticles;
                    --mNumFreeParticles;
                    return &particle;
                }
            }
        }
        return nullptr;
    }

    void ParticleContainer::KillParticle(Particle& particle)
    {
        if (mOneShot)
        {
            particle.lifetime = -1.0f;
        }
        else
        {
            particle.lifetime = 0.0f;
            ++mNumFreeParticles;
        }
        --mNumActiveParticles;
    }
}  // namespace mts