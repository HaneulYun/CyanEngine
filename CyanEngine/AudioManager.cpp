#include "pch.h"
#include "AudioManager.h"

using namespace FMOD;

AudioManager::AudioManager()
{
	System_Create(&system);
	system->init(64, FMOD_INIT_NORMAL, NULL);

	system->getMasterChannelGroup(&groupMaster);
	
	AddGroup("groupTest");
	AddSound("soundTest", "Assets\\FootstepSound\\Grass\\test.mp3");
	
	Channel* channel;
	system->playSound(clips["soundTest"]->sound, nullptr, 0, &channel);
}

AudioManager::~AudioManager()
{
	delete system;
}

void AudioManager::update()
{
	system->update();
}

void AudioManager::AddGroup(std::string name)
{
	system->createChannelGroup(name.c_str(), &mixerGroups[name]->group);
}

void AudioManager::AddSound(std::string name, std::string path)
{
	if (clips[name])
		return;
	clips[name] = new AudioClip();
	system->createSound(path.c_str(), FMOD_DEFAULT, 0, &clips[name]->sound);
}
