#pragma once

class ParticleSystem : public MonoBehavior<ParticleSystem>
{
private:

public:
	Mesh* particle;

private:
	friend class GameObject;
	friend class MonoBehavior<ParticleSystem>;
	ParticleSystem() = default;
	ParticleSystem(ParticleSystem&) = default;

public:
	~ParticleSystem() {}

	void Start()
	{
	}

	void Update()
	{
	}
};