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

	template <>
	GameObject* Instantiate(GameObject* original);
};

