#pragma once
#include "Core/Graphics/Sprite.hpp"
#include "Particle/ParticleList.h"

namespace LuaSTGPlus::Particle
{
	class ParticlePool2D
	{
	public:
		ParticlePool2D(int32_t size) : plist(size) {}
		void Update();
		void Render();
	public:
		struct Particle
		{
		public:
			Core::Vector2F pos;
			Core::Vector2F vel;
			Core::Vector2F accel;
			Core::Vector2F scale;

		private:
			
		};

		void AddParticle(Particle p);
	private:
		ParticleList<Particle> plist;
	};
}

