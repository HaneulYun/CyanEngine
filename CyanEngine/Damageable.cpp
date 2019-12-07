#include "pch.h"
#include "scripts.h"
#include "Message.h"

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
	Damager* damager = collision->GetComponent<Damager>();

	if (damager != NULL)
	{
		if (damager->CanDamage())
		{
			// Bullet Disable
			damager->DisableDamage();

			// Push Msg Queue
			Message message;
			message.msgId = MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY;
			message.rParam = damager->GetDamageAmount();
			sendQueue.push(message);
		}

		// 서버에서 충돌 메시지가 다시 돌아오면 그 때 TakeDamage . 
		//TakeDamage(*collision->GetComponent<Damager>());
	}
}