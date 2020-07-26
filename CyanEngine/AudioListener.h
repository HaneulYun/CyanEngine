#pragma once

class AudioListener : public MonoBehavior<AudioListener>
{
private:

public:

protected:
	friend class GameObject;
	friend class MonoBehavior<AudioListener>;
	AudioListener() = default;
	AudioListener(AudioListener&) = default;

public:
	~AudioListener() {}

	void Start()
	{
	}
	
	void Update()
	{
		auto matrix = gameObject->GetMatrix();
		
		FMOD_VECTOR pos = matrix.position.fmodVector;
		FMOD_VECTOR forward = matrix.forward.fmodVector;
        FMOD_VECTOR up = matrix.up.fmodVector;
		FMOD_VECTOR vel{};

		AudioManager::Instance()->system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	}
};