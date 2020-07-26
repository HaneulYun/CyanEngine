#pragma once

class AudioManager : public Singleton<AudioManager>
{
	FMOD_SYSTEM* system;

public:
	AudioManager();
};
