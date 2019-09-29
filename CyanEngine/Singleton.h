#pragma once

template <typename T>
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

template <typename T> T* Singleton<T>::instance = nullptr;