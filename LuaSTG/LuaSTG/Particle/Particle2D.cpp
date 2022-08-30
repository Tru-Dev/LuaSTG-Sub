#include "Particle/Particle2D.h"


namespace LuaSTGPlus::Particle
{
    void ParticlePool2D::AddParticle(ParticlePool2D::Particle p)
    {
        plist.insert(p);
    }
}