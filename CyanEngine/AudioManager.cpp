#include "pch.h"
#include "AudioManager.h"

using namespace FMOD;

AudioManager::AudioManager()
{
	System_Create(&system);
	system->init(64, FMOD_INIT_NORMAL, NULL);

	system->getMasterChannelGroup(&groupMaster);
}

AudioManager::~AudioManager()
{
	delete system;
}

void AudioManager::update()
{
	system->update();
}

void AudioManager::CreateGroup(AudioClip* outputAudioMixerGroup)
{
	//system->createChannelGroup(name.c_str(), &mixerGroups[name]->group);
}

void AudioManager::CreateSound(AudioClip* clip)
{
	system->createSound(clip->path.c_str(), FMOD_DEFAULT, 0, &clip->sound);
}
