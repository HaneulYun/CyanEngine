#pragma once

class Damageable : public Component
{
private:
	int startingHealth;
	int health;

public:
	bool isTeam;

public:
	Damageable();
	Damageable(int hp);
	~Damageable();

	void Start() override {}
	void Update() override {}

	virtual Component* Duplicate() { return new Damageable; };
	virtual Component* Duplicate(Component* component) { return new Damageable(*(Damageable*)component); }

	void TakeDamage(Damager damager);
	void TakeDamage(int damageAmount);

	void SetHealth(int amount);
	int GetCurHealth();
	void ResetHealth();
	bool isDead();

	void OnTriggerEnter(GameObject* damager);
};