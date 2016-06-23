/***********************************************
* Filename:  		VisualEffectsManager.h
* Date:      		3/9/16
* Mod. Date: 		3/9/16
* Mod. Initials:	DKR
* Author:    		David Rasmussen
* Purpose:
************************************************/
#ifndef Visual_Effects_Manager_h
#define Visual_Effects_Manager_h
/***********************************************/

// Includes

// Forward Declarations 

class CVisualEffectsManager
{

private:

	// Members

	// Helper Functions

public:

	// Constructors
	CVisualEffectsManager();

	// Destructor
	~CVisualEffectsManager();

	// Accessors

	// Mutators

	// Methods
	void Initialize();
	void Update(float _deltaTime);
	void ProcessEvent(void* _data);
	void Shutdown();

};

#endif