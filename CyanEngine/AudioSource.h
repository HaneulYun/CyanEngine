#pragma once

class AudioSource : public MonoBehavior<AudioSource>
{
private:

public:
	FMOD::Channel* channel{ nullptr };

protected:
	friend class GameObject;
	friend class MonoBehavior<AudioSource>;
	AudioSource() = default;
	AudioSource(AudioSource&) = default;

public:
	~AudioSource() {}

	void Start()
	{
	}

	void Update()
	{
	}
};