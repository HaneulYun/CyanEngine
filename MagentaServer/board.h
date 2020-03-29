#pragma once
#include "framework.h"

struct Vector3 {
	float x, y, z;
};

class Pawn {
private:
	int x, y;

public:
	Pawn() { x = 0, y = 0; }
	Pawn(int xpos, int ypos) { x = xpos, y = ypos; }

	int getX() const { return x; }
	int getY() const { return y; }

	void moveUp() { if (y < 7) ++y; }
	void moveDown() { if (y > 0) --y; }
	void moveLeft() { if (x > 0) --x; }
	void moveRight() { if (x < 7) ++x; }
	void move(float& xpos, float& ypos) {
		if (xpos > -300 && xpos < 300) {
			x = (xpos + 300) / (600 / 8);
			y = (ypos + 300) / (600 / 8);
		}
	}
};

class Board {
public:
	Pawn p;

public:
	Vector3 getPosToBoardPos(const int& x, const int& y)
	{
		Vector3 boardPos;
		boardPos.x = x * 600 / 8 - (2100 / 8);
		boardPos.y = y * 600 / 8 - (2100 / 8);
		boardPos.z = -1;

		return boardPos;
	}
};