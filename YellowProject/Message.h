#pragma once
#include "framework.h"

// server to client
struct Message {
	char msgID;
	float x, y, z;
};

#define MOVE 0x00

// client to server
#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04

