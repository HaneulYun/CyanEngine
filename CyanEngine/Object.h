#pragma once

class GameObject;

class Object
{
public:
	Object() {}
	~Object() {}

	template <typename T>
	T* Instantiate(T* original)
	{
		T* instance = new T(original);
		return instance;
	}

	GameObject* Instantiate(GameObject* original);
	void Destroy(GameObject* obj);
};

