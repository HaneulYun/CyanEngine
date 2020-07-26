#pragma once

struct AudioClip
{
	std::string name;
	std::string path;
	FMOD::Sound* sound{ nullptr };
};

struct AudioMixerGroup
{
	FMOD::ChannelGroup* group{ nullptr };
};

class AudioManager : public Singleton<AudioManager>
{
public:
	FMOD::System* system{ nullptr };

	FMOD::ChannelGroup* groupMaster{ nullptr };

public:
	AudioManager();
	~AudioManager();

	void update();

	void CreateGroup(AudioClip* outputAudioMixerGroup);
	void CreateSound(AudioClip* clip);
};
