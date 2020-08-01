#pragma once

class AudioSource : public MonoBehavior<AudioSource>
{
private:

public:
	AudioClip* clip{ nullptr };
	FMOD::Channel* channel{ nullptr };

	bool playOnAwake{ true };

	bool spatial{ false };
	float minDistance{ 0 };
	float maxDistance{ 100 };

	bool loop{ false };

protected:
	friend class GameObject;
	friend class MonoBehavior<AudioSource>;
	AudioSource() = default;
	AudioSource(AudioSource&) = default;

public:
	~AudioSource() {}

	void Start()
	{
		if(loop)
			clip->sound->setMode(FMOD_LOOP_NORMAL);
		if (spatial)
		{
			clip->sound->setMode(FMOD_3D_LINEARSQUAREROLLOFF);
			clip->sound->setMode(FMOD_3D);
		}
		if(playOnAwake)
			Play();
	}

	void Update()
	{
		if (channel)
		{
			Matrix4x4 matrix = gameObject->GetMatrix();
			FMOD_VECTOR pos = matrix.position.fmodVector;
			FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

			channel->set3DAttributes(&pos, &vel);
		}
	}

	void Play()
	{
		clip->sound->set3DMinMaxDistance(minDistance, maxDistance);

		AudioManager::Instance()->system->playSound(clip->sound, nullptr, false, &channel);
	}
};