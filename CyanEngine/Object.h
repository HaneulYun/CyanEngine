#pragma once

class Object
{
public:
	Object() {}
	~Object() {}

	template <typename T>
	static T* Instantiate(T* original)
	{
		T* instance = new T(original);
		return instance;
	}
};

