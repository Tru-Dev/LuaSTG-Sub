#include "Particle/Particle2D.h"


namespace LuaSTGPlus::Particle
{
    void ParticlePool2D::Update()
    {
        for (ParticlePool2D::Particle& p : plist)
        {
            p.vel += p.accel;
            p.pos += p.vel;
            p.rot += p.omiga;
            p.timer += 1;
        }
    }

    void ParticlePool2D::Render()
    {
        for (ParticlePool2D::Particle& p : plist)
        {
            Core::Graphics::ISprite* pImg = *img;
            pImg->setColor(p.color);
            pImg->draw(p.pos, p.scale, p.rot);
        }
    }

    void ParticlePool2D::Apply(std::function<bool(ParticlePool2D::Particle* const)> fn)
    {
        plist.foreach(fn);
    }
}