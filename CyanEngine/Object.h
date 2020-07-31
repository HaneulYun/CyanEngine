#pragma once

class GameObject;

class Object
{
public:
	std::string name;

	Object() {}
	~Object() { OnDestroy(); }

	template <typename T>
	T* Instantiate(T* original)
	{
		T* instance = new T(original);
		return instance;
	}

	virtual void OnDestroy() {}

	GameObject* Instantiate(GameObject* original);
	void Destroy(GameObject* obj);
};

