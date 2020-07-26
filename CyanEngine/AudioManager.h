#pragma once

struct AudioClip
{
	std::string name;
	FMOD::Sound* sound{ nullptr };
};

struct AudioMixerGroup
{
	FMOD::ChannelGroup* group{ nullptr };
};

class AudioManager : public Singleton<AudioManager>
{
	FMOD::System* system{ nullptr };

	FMOD::ChannelGroup* groupMaster{ nullptr };
	std::map<std::string, AudioMixerGroup*> mixerGroups;
	std::map<std::string, AudioClip*> clips;

public:
	AudioManager();
	~AudioManager();

	void update();

	void AddGroup(std::string name);
	void AddSound(std::string name, std::string path);
};
