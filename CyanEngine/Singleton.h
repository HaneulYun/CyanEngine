#pragma once

template <typename Type, typename T>
class Singleton
{
private:
	static T* instance;

protected:
	Singleton() {}
	~Singleton()
	{
		if (instance) delete instance;
		instance = nullptr;
	}

public:
	static T* Instance()
	{
		if (!instance) instance = new T();
		return instance;
	}
};

template <typename Type, typename T> T* Singleton<Type, T>::instance = nullptr;