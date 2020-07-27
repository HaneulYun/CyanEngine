#pragma once

class AudioSource : public MonoBehavior<AudioSource>
{
private:

public:
	AudioClip* clip{ nullptr };
	FMOD::Channel* channel{ nullptr };

	bool playOnAwake{ true };
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
		clip->sound->setMode(FMOD_3D_LINEARSQUAREROLLOFF);
		if(loop)
			clip->sound->setMode(FMOD_LOOP_NORMAL);
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
		clip->sound->set3DMinMaxDistance(0, 100);

		AudioManager::Instance()->system->playSound(clip->sound, nullptr, false, &channel);
	}
};