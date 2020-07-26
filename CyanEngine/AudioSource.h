#pragma once

class AudioSource : public MonoBehavior<AudioSource>
{
private:

public:
	AudioClip* clip{ nullptr };
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
		AudioManager::Instance()->system->playSound(clip->sound, nullptr, 0, &channel);
	}
};