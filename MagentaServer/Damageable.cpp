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
	if (damager.isTeam == isTeam)
		return;
	if (health <= 0)
		return;

	health -= damager.GetDamageAmount();
}

void Damageable::TakeDamage(int damageAmount)
{
	if (health <= 0)
		return;

	health -= damageAmount;
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

void Damageable::OnTriggerEnter(GameObject* collision)
{
	if (collision->GetComponent<Damager>() != NULL)
	{
		TakeDamage(*collision->GetComponent<Damager>());
	}
}