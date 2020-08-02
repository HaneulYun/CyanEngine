#pragma once

template <typename T>
class Singleton
{
protected:
	static T* instance;

protected:
	Singleton() {}
	virtual ~Singleton()
	{
		instance = nullptr;
	}

public:
	static T* Instance()
	{
		if (!instance) instance = new T();
		return instance;
	}
};

template <typename T> T* Singleton<T>::instance = nullptr;
