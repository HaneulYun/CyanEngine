#include "pch.h"
#include "Damager.h"

Damager::Damager()
{

}

Damager::Damager(int damage)
{
	damageAmount = damage;
}

Damager::~Damager()
{

}

void Damager::SetDamageAmount(int amount)
{
	damageAmount = amount;
}

int Damager::GetDamageAmount()
{
	return damageAmount;
}

void Damager::AddDamageTo(Damageable damageable)
{
	if (!canDamage)
		return;
	damageable.TakeDamage(*this);
	DisableDamage();
}

void Damager::EnableDamage()
{
	canDamage = true;
}

void Damager::DisableDamage()
{
	canDamage = false;
}