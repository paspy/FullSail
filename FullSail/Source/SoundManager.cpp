#include "pch.h"
#include "SoundManager.h"
#include "Wwise_IDs.h"

#define VOLUME_MAX	((float)100.0f)		// default: 100
#define VOLUME_MIN	((float)0.0f)		// default: 0
#define VOLUME_AMT	((float)0.10f)		// default: 0.10


SoundManager::SoundManager()
{
	volume = 0.0f;	// default: 50
	audioScaling = 0.0f;	// default: 1
}


SoundManager::~SoundManager()
{
	//Shutdown();
}


bool SoundManager::Initialize(void)
{
	// init system
	if (audioSystem.Initialize() == false)
		return false;

	// set dafault path
	audioSystem.SetBasePath(L"../../FullSail/Resources/SoundBank/");

	// load banks
	if (audioSystem.LoadSoundBank(L"Init.bnk") == false)
		return false;
	if (audioSystem.LoadSoundBank(L"SoundBank.bnk") == false)
		return false;

	// set world scale
	audioSystem.SetWorldScale(1.0f);

	volume = 50.0f;
	audioScaling = 1.0f;


	return true;
}


void SoundManager::Shutdown(void)
{
	audioSystem.UnloadSoundBank(L"Init.bnk");
	audioSystem.UnloadSoundBank(L"SoundBank.bnk");
	audioSystem.Shutdown();
}


void SoundManager::Update(float _dt)
{
	audioSystem.Update();
	
}


// Volume Methods
void SoundManager::VolumeUp(AkUniqueID _id)
{
	if (volume < VOLUME_MAX)
		volume += VOLUME_AMT;
}


void SoundManager::VolumeDown(AkUniqueID _id)
{
	if (volume > VOLUME_MIN)
		volume -= VOLUME_AMT;
}



// Play Sound Methods
void SoundManager::Play3DSound(AkUniqueID _id)
{
	audioSystem.PostEvent(_id);
	//audioSystem.PostEvent(_id, XMFLOAT3(0, 0, 0));
}


void SoundManager::Play3DSound(AkUniqueID _id, XMFLOAT3 _pos)
{
	audioSystem.PostEvent(_id, _pos);
}


void SoundManager::Play3DSound(AkUniqueID _id, IEntity* _e)
{
	audioSystem.PostEvent(_id, _e);
}


void SoundManager::Play3DSound(AkUniqueID _id, unsigned int _listener)
{
	audioSystem.PostEvent(_id, _listener);
}


void    SoundManager::PostEvent(AkUniqueID _id)
{
	audioSystem.PostEvent(_id);
}


void SoundManager::StopSounds(void)
{
	audioSystem.PostEvent(AK::EVENTS::STOP_ALL);
}


void SoundManager::PauseSounds(void)
{
	audioSystem.PostEvent(AK::EVENTS::PAUSE_ALL);
}


void SoundManager::ResumeSounds(void)
{
	audioSystem.PostEvent(AK::EVENTS::RESUME_ALL);
}


void SoundManager::SetStates(const AkStateGroupID akStateGroupID, const AkStateID akStateID)
{
	audioSystem.SetState(akStateGroupID, akStateID);
}

