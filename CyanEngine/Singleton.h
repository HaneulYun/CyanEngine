#pragma once

template <typename T>
class Singleton
{
private:
	static T* instance{ nullptr };

	Singleton();

public:
	inline T* Singleton<T>::Instance()
	{
		if (!instance) instance = new T();
		return instance;
	}
};