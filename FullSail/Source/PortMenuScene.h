/***************************************************************
|	File:		PortMenuScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	PortMenuScene class used to store the port menu data
***************************************************************/
#pragma once

#include "IScene.h"
#include <string>
#include "IShipPart.h"
//#include "GUIManager.h"
#include "GUIAnimation.h"

#define NUM_PORT_MENU_PAGES		((int)3)
#define NUM_CHOICES_PAGE_1		((int)5)
#define NUM_CHOICES_PAGE_2		((int)4)
#define NUM_CHOICES_PAGE_3		((int)4)
//#define NUM_CHOICES_PAGE_4	((int)2)
#define NUM_RESOURCE_INCREMENTS	((int)4)
#define COLOR_CHANGE_TIMER		((float)1.0f)

class Port;


class PortMenuScene : public IScene
{
	// feedback structs
	struct FeedbackColorChange
	{
		G2D::GUIText*		text;
		SimpleMath::Color	startColor;
		SimpleMath::Color	currColor;
		SimpleMath::Color	endColor;
		float				colorTimer;

		FeedbackColorChange(G2D::GUIText * t, SimpleMath::Color sc = { 1.0f, 1.0f, 0.0f, 1.0f }, SimpleMath::Color ec = { 0.0f, 0.0f, 0.0f, 1.0f })
			: text(t), startColor(sc), currColor(sc), endColor(ec), colorTimer(COLOR_CHANGE_TIMER) {}
		~FeedbackColorChange() { text = nullptr; }

		void Restart(void)
		{
			colorTimer = 0.0f;
		}
	};
	struct DescriptionStruct
	{
		std::wstring text;
		SimpleMath::Color color;

		DescriptionStruct(void) : text(L"N/A"), color({ 0.0f, 0.0f, 0.0f, 0.0f }) {}
		DescriptionStruct(std::wstring t, SimpleMath::Color c) : text(t), color(c) {}
	};


public:
	enum PortItems { UNKNOWN = -1, WATER_FOOD = 0, RUM, CREW, REPAIR, WEAPONS, SAILS, HULL, FLAMETHROWER, BARRELS, GREEKSHOT, FLEET, EXIT };
	//enum PortItems { UNKNOWN = -1, WEAPONS = 0, HULL, SAILS, REPAIR, WATER_FOOD, RUM, CREW, EXIT, FLEET, FLAMETHROWER, BARRELS, GREEKSHOT, NUM_ITEMS };
	//enum PortItems { UNKNOWN = -1, WEAPONS = 0, HULL, SAILS, REPAIR, WATER_FOOD, RUM, CREW, EXIT, FLEET };

	// Event structs
	struct CursorUpDownEventArgs
	{
		int			numChoices;
		bool		keyboard;

		CursorUpDownEventArgs(void) : numChoices(0), keyboard(false) {}
		CursorUpDownEventArgs(int nc, bool k) : numChoices(nc), keyboard(k) {}
	};
	struct CursorLeftRightEventArgs
	{
		PortItems	item;
		bool		keyboard;

		CursorLeftRightEventArgs(void) : item(PortItems::UNKNOWN), keyboard(false) {}
		CursorLeftRightEventArgs(PortItems pi, bool k) : item(pi), keyboard(false) {}
	};
	struct PortSetupEventArgs
	{
		Port*		currPort;

		PortSetupEventArgs(void) : currPort(nullptr) {}
		PortSetupEventArgs(Port* p) : currPort(p) {}
	};


	PortMenuScene();
	~PortMenuScene();


	void		Enter	(void);
	void		Exit	(void);

	bool		Input	(void);
	void		Update	(float _dt);
	void		Render	(void);

	void		Input_Start	(void);
	void		Input_End	(void);

	bool		Input_Keyboard	(void);
	bool		Input_Gamepad	(void);

    void        LoadHudList();
    void        ReleaseHudList();

	// Helper functions
	PortItems	GetCurrPortItem			(void);
	bool		OnExit					(void);
    void		ResetPrices				(void);
    void		ResetPrice				(PortItems item) { prices[(int)item] = baseprices[(int)item]; }
	int			GetBasePrice			(PortItems item) { return baseprices[(int)item]; }
	int			GetCurrentPrice			(PortItems item, int level);
	int			CalculateLoweredPrice	(int baseprice, int curr, int max);
	int			CalculateDiscountPrice	(int baseprice, int discount);
    void		SetupStandardPrices		(void);
    void		SetupDiscountPrices		(Port* p);
    void		DeactivateToggleableGUI	(void);
    void		ActivatePage1GUI		(void);
    void		ActivatePage2GUI		(void);
    void		ActivatePage3GUI		(void);
    void		ActivateItemDescriptions(PortItems item);



	// Feedback functions
    void		StartFeedbackColorChange	(std::string guiname);
    void		UpdateFeedbackElements		(float dt);
    void		ReleaseFeedbackElements		(void);


	// Purchase functions
    void		Upgrade			(PortItems item, IShipPart::PartType part);
	void		Repair			(void);
	void		Refill			(PortItems item);
	void		SpecialWeapons	(PortItems item);
	void		FleetShip		(void);


	// Page input functions
	bool		New_Input_Keyboard		(void);
	bool		Page1_Input_Keyboard	(void);
	bool		Page2_Input_Keyboard	(void);
	bool		Page3_Input_Keyboard	(void);
	bool		Page4_Input_Keyboard	(void);
	bool		New_Input_Gamepad		(void);
	bool		Page1_Input_Gamepad		(void);
	bool		Page2_Input_Gamepad		(void);
	bool		Page3_Input_Gamepad		(void);
	bool		Page4_Input_Gamepad		(void);


	// Event funtions
	void		PortSetupEvent			(const EventArgs1<PortSetupEventArgs>& args);
	void		CursorIncreaseEvent		(const EventArgs1<CursorUpDownEventArgs>& args);
	void		CursorDecreaseEvent		(const EventArgs1<CursorUpDownEventArgs>& args);
	void		CursorIncrementEvent	(const EventArgs1<CursorLeftRightEventArgs>& args);
	void		CursorDecrementEvent	(const EventArgs1<CursorLeftRightEventArgs>& args);


	// Accessors
	std::wstring	GetPortName	(void)	const	{ return portName; }


	// Mutators
	void			SetPortName	(std::wstring _n)		{ portName = _n; }


private:

	// Port Menu selection
	int				currPage		= 0;
	int				numPagesOpen	= 0;
	int				currCursor		= 0;
	float			scrollTimer		= 0.0f;
	bool			scrollStuck		= false;


	// Port stuff
	std::wstring	portName;
	int				baseprices[11];		// standard prices
	int				prices[11];			// current prices
	int				priceOffsets[5];	// upgrade price offsets
	bool			onsale;
	int				discounts[11];		// port discounts


	// resource increment/decrement
	int				resourceIncrements[3];						// (0) <= (resourceIncrements[n]) < (NUM_RESOURCE_INCREMENTS)
	int				incrementNums[NUM_RESOURCE_INCREMENTS];		// currResource += incrementNums[resourceIncrements[n]]
	
	
	// GUI/HUD
	std::unordered_map<std::string, G2D::Entity2D*>		m_hudElems;
	std::map<std::string, FeedbackColorChange*>			m_feedbackElems;
	G2D::GUIAnimationMap								m_hudAnims;
};
