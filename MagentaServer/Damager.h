#pragma once

class Damageable;

class Damager : public Component
{
private:
	int damageAmount;
	bool canDamage;

public:
	bool isTeam;

public:
	Damager();
	Damager(int damage);
	~Damager();

	void Start() override {}
	void Update() override {}
	void Destroy() override {}

	virtual Component* Duplicate() { return new Damager; };
	virtual Component* Duplicate(Component* component) { return new Damager(*(Damager*)component); }

	void SetDamageAmount(int amount);
	int GetDamageAmount();
	void AddDamageTo(Damageable damageable);
	void EnableDamage();
	void DisableDamage();

};