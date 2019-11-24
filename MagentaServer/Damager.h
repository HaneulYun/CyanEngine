#pragma once

class Damageable;

class Damager : public Component
{
private:
	int damageAmount;
	bool canDamage;
	// collider 변수 추가..?

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