#pragma once

class GameScene : public Scene
{
public:
	GameScene() : Scene() {}
	virtual ~GameScene() {}

	virtual void BuildObjects();
};