/***********************************************
* Filename:  		VisualEffect.h
* Date:      		3/9/16
* Mod. Date: 		3/9/16
* Mod. Initials:	DKR
* Author:    		David Rasmussen
* Purpose:
************************************************/
#ifndef Visual_Effect_h
#define Visual_Effect_h
/***********************************************/

// Includes

// Forward Declarations 

class CVisualEffect
{

private:

	// Members

	// Helper Functions

public:

	// Constructors
	CVisualEffect();

	// Destructor
	~CVisualEffect();

	// Accessors

	// Mutators

	// Methods
	void Create();
	void Play();
	void Stop();
	void Destroy();
	void Update(float _deltaTime);

};

#endif

