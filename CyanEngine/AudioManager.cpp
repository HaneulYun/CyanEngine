#include "pch.h"
#include "AudioManager.h"

using namespace FMOD;

AudioManager::AudioManager()
{
	System_Create(&system);
	system->init(64, FMOD_INIT_NORMAL, NULL);

	system->getMasterChannelGroup(&groupMaster);
	
	//AddGroup("groupTest");
	//AddSound("soundTest", "test.mp3");
	//
	//Channel* channel;
	//system->playSound(sounds["soundTest"], groups["groupTest"], 0, &channel);
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
	system->createChannelGroup(name.c_str(), &groups[name]);
}

void AudioManager::AddSound(std::string name, std::string path)
{
	system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sounds[name]);
}
