#pragma once

struct Message
{
	unsigned char msgId;
	int lParam;
	float mParam;
	float rParam;
};

//////////////////////////////////////////////
// Client to Server
#define MESSAGE_READY 0x90	// lParam = 보내는 클라의 아이디

// mParam = 별 수호자 ID / rParam = 방향(radian)
#define MESSAGE_REQUEST_BULLET_CREATION 0x80	
#define MESSAGE_REQUEST_BULLET_CREATION_STRAIGHT 0x81
#define MESSAGE_REQUEST_BULLET_CREATION_CANNON 0x82
#define MESSAGE_REQUEST_BULLET_CREATION_SHARP 0x83
#define MESSAGE_REQUEST_BULLET_CREATION_LASER 0x84
#define MESSAGE_REQUEST_BULLET_CREATION_GUIDED 0x85

// lParam = BulletID / mParam = EnemyID / rParam = Damage
#define MESSAGE_NOTIFY_COLLISION_BULLET_AND_ENEMY 0x60


//////////////////////////////////////////////
// Server to Clients
#define MESSAGE_YOUR_ID 0x00	// lParam = 클라의 아이디
#define MESSAGE_CONNECTED_IDS 0x01 // 서버와 연결된 클라이언트들 ID 반환.
#define MESSAGE_GAME_START 0x02	
#define MESSAGE_GAME_END 0x03

// lParam = ObjectID / mParam = 별 수호자 ID / rParam = 방향(radian)
#define MESSAGE_CREATE_BULLET 0x10
#define MESSAGE_CREATE_BULLET_STRAIGHT 0x11
#define MESSAGE_CREATE_BULLET_CANNON 0x12
#define MESSAGE_CREATE_BULLET_SHARP 0x13
#define MESSAGE_CREATE_BULLET_LASER 0x14
#define MESSAGE_CREATE_BULLET_GUIDED 0x15

// lParam = ObjectID / mParam = None / rParam = 방향(radian)
//#define MESSAGE_CREATE_ENEMY 0x20
#define MESSAGE_CREATE_ENEMY_COMINGRECT 0x20
#define MESSAGE_CREATE_ENEMY_ROTATINGRECT 0x21
#define MESSAGE_CREATE_ENEMY_COMINGBIGRECT 0x22
#define MESSAGE_CREATE_ENEMY_COMINGTWOTRIANGLE 0x23
#define MESSAGE_CREATE_ENEMY_WHIRLINGCOMINGRECT 0x24

// lParam = enemyID 
#define MESSAGE_DELETE_ENEMY 0x30

// Collision Message
#define MESSAGE_NOTIFY_COLLISION_STAR_AND_ENEMY 0x40	// lParam = enemy ID, mParam = Star HP
