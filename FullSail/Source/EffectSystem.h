/***********************************************
* Filename:  		EffectSystem.h
* Date:      		3/9/16
* Mod. Date: 		3/9/16
* Mod. Initials:	DKR
* Author:    		David Rasmussen
* Purpose:   		
************************************************/
#ifndef Effect_System_h
#define Effect_System_h
/***********************************************/

// Includes

// Forward Declarations 

class CEffectSystem
{

private:

	// Members

	// Helper Functions

public:

	// Constructors
	CEffectSystem();

	// Destructor
	~CEffectSystem();

	// Accessors

	// Mutators

	// Methods
	void Create();
	void Play();
	void Stop();
	void Destroy();
	void Update( float _deltaTime );

};

#endif
