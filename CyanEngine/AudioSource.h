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
		clip->sound->set3DMinMaxDistance(0.5, 100);

		AudioManager::Instance()->system->playSound(clip->sound, nullptr, false, &channel);

		Matrix4x4 matrix = gameObject->GetMatrix();
		FMOD_VECTOR pos = matrix.position.fmodVector;
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		channel->set3DAttributes(&pos, &vel);
	}
};