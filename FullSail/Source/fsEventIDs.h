#pragma once
typedef std::string EventID;



enum Scenes { UNKNOWN = -1, INTRO = 0, MAIN_MENU, GAMEPLAY, OPTIONS,
	INSTRUCTIONS, PORT, PAUSE, WIN, LOSE, CREDITS, NUM_SCENES };

namespace fsEvents
{
	// AI events
	static const EventID AI_Alert = "Alert";
	static const EventID AI_Help = "I Need an Adult";

	// Cannon firing events
	static const EventID FireLeftCannons = "FireLeftSideCannonEvent";
	static const EventID FireRightCannons = "FireRightSideCannonEvent";
	static const EventID FireSideCannons = "FireBothSideCannonsEvent";
	static const EventID FireFrontCannon = "FireFrontCannonEvent";

	// Port events
	static const EventID PortSetColor = "PortSetColorEvent";
	static const EventID PortSale = "PortSaleEvent";
	static const EventID PortSetup = "PortSetupEvent";
	static const EventID PortCursorUp = "PortCursorUpEvent";
	static const EventID PortCursorDown = "PortCursorDownEvent";
	static const EventID PortCursorLeft = "PortCursorLeftEvent";
	static const EventID PortCursorRight = "PortCursorRightEvent";

	//Entity Manager
	static const EventID AddEntity = "AddEntity";
	static const EventID LoadObject = "LoadObject";

	//GameplayScene
	static const EventID SpawnAI = "SpawnAI";
	static const EventID SpawnReconSquad = "SpawnBossReconSquadEvent";


	//Boss Events
	static const EventID SpawnBossReinforcementSquad = "SpawnBossReinforcementSquadEvent";
	static const EventID BossTracking = "BossTrackingPlayerEvent";
	static const EventID BossHealing = "BossHealingEvent";
	//static const EventID BossRethinkTrackingCourse = "BossRethinkTrackingCourseEvent";
	//static const EventID BossStayOnTrackingCourse = "BossStayOnTrackingCourseEvent";
	//static const EventID BossNextWaypoint = "BossNextWaypointEvent";




#pragma region //Steering Manager

	//const EventArgs1<Boid*>&
	static const EventID AddBoid = "AddBoid";
	
	//const EventArgs1<Boid*>&
	static const EventID DeleteBoid = "DeleteBoid";
	
	//const EventArgs1<Squad*>& newEvent
	static const EventID AddSquad = "AddSquad";
	
	//const EventArgs1<Squad*>& newEvent
	static const EventID RemoveSquad = "RemoveSquad";

	//const EventArgs1<Boid*>&
	static const EventID AddSquadmate = "Squadmate";

	//const EventArgs2<Boid*, Boid*>&
	static const EventID FollowLeader = "Following";

	//const EventArgs2<int, Boid*>&
	static const EventID LeaveSquad = "LeaveSquad";

	//const EventArgs2<Boid*, Boid*>&
	static const EventID StopFollowing = "StopFollowing";
#pragma endregion

}