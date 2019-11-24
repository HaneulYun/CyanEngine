#include "pch.h"
#include "Damageable.h"

Damageable::Damageable()
{
}

Damageable::Damageable(int hp)
{
	startingHealth = hp;
	health = startingHealth;
}

Damageable::~Damageable()
{
}

void Damageable::TakeDamage(Damager damager)
{
	if (health <= 0)
		return;

	health -= damager.GetDamageAmount();
	printf("¾Æ¾ß!\n");
}

void Damageable::SetHealth(int amount)
{
	health = amount;
}

int Damageable::GetCurHealth()
{
	return health;
}

void Damageable::ResetHealth()
{
	health = startingHealth;
}

bool Damageable::isDead()
{
	if (health <= 0)
		return true;
	return false;
}
