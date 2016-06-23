/***************************************************************
|	File:		SoundManager.h
|	Author:		Jason N. Bloomfield
|	Created:	March 9, 2016
|	Modified:	March 10, 2016
|	Purpose:	SoundManager class used to play sound & interact
|				-with WWise
***************************************************************/
#pragma once

#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"


class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	bool			Initialize(void);
	void			Shutdown(void);
	bool			GetInitialized(void)	const { return audioSystem.GetInitialized(); }

	void			Update(float _dt);

	// Volume Methods
	void			VolumeUp(AkUniqueID _id);
	void			VolumeDown(AkUniqueID _id);

	// Play Sound Methods
	void			Play3DSound(AkUniqueID _id);
	void			Play3DSound(AkUniqueID _id, XMFLOAT3 _pos);
	void			Play3DSound(AkUniqueID _id, IEntity* _e);
	void			Play3DSound(AkUniqueID _id, unsigned int _listener);
	void			PostEvent(AkUniqueID _id);
	void			StopSounds(void);
	void			PauseSounds(void);
	void			ResumeSounds(void);

	// Register Methods
	bool			RegisterListener(IEntity* _e, std::string _n) { return audioSystem.RegisterListener(_e, _n.c_str()); }
	bool			RegisterEntity(IEntity* _e, std::string _n) { return audioSystem.RegisterEntity(_e, _n.c_str()); }
	bool			UnregisterListener(IEntity* _e) { return audioSystem.UnRegisterListener(_e); }
	bool			UnregisterEntity(IEntity* _e) { return audioSystem.UnRegisterEntity(_e); }

	// States Methods
	void SetStates(const AkStateGroupID akStateGroupID, const AkStateID akStateID);

	AudioSystemWwise	audioSystem;
private:

	float				volume;
	float				audioScaling;

};
