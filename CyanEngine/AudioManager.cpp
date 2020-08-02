#include "pch.h"
#include "AudioManager.h"

using namespace FMOD;

AudioManager::AudioManager()
{
	System_Create(&system);
	system->init(64, FMOD_INIT_NORMAL, NULL);
	system->set3DSettings(1, 1, 1);
	
	system->getMasterChannelGroup(&groupMaster);
}

AudioManager::~AudioManager()
{
	system->close();
	system->release();
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
	FMOD_MODE mode{};
	mode = FMOD_2D;
	system->createSound(clip->path.c_str(), mode, 0, &clip->sound);
}
