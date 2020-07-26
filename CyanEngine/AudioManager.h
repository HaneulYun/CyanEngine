#pragma once

class AudioManager : public Singleton<AudioManager>
{
	FMOD::System* system{ nullptr };

	FMOD::ChannelGroup* groupMaster{ nullptr };
	std::map<std::string, FMOD::ChannelGroup*> groups;
	std::map<std::string, FMOD::Sound*> sounds;

public:
	AudioManager();
	~AudioManager();

	void update();

	void AddGroup(std::string name);
	void AddSound(std::string name, std::string path);
};
