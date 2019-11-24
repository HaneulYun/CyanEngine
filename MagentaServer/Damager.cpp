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
	// 종류가 CANNON일 경우 충돌한 모든 객체에 피해를 주어야함
	// CANNON 구현 완료되면 추가하기
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