#ifndef GameStatics_h__
#define GameStatics_h__

#define KNOT_2_FOOT_PER_SEC   1.68781f

class GameStatistics {
public:
    static GameStatistics & GetRef() { static GameStatistics instance; return instance; }

    float RegularShotRate() const;
    float ChainShotRate() const;
    unsigned int GetFinalScore() const;

    // NOTE: These are INCREMENTAL!!
    void RegularShotFired   ( unsigned int val ) { m_unRegularShotFired += val; }
    void RegularShotHit     ( unsigned int val ) { m_unRegularShotHit += val; }
    void ChainShotFired     ( unsigned int val ) { m_unChainShotFired += val; }
    void ChainShotHit       ( unsigned int val ) { m_unChainShotHit += val; }

    unsigned int FoodShipDestroyed() const              { return m_unFoodShipDestroyed; }
    unsigned int GoldShipDestroyed() const              { return m_unGoldShipDestroyed; }
    unsigned int MilitaryShipDestroyed() const          { return m_unMilitaryShipDestroyed; }

    // NOTE: These are INCREMENTAL!!
    void FoodShipDestroyed      ( unsigned int val )    { m_unFoodShipDestroyed += val; }
    void GoldShipDestroyed      ( unsigned int val )    { m_unGoldShipDestroyed += val; }
    void MilitaryShipDestroyed  ( unsigned int val )    { m_unMilitaryShipDestroyed += val; }

    unsigned int CrateCollected() const         { return m_unCrateCollected; }
    unsigned int FoodCollected() const          { return m_unFoodCollected; }
    unsigned int GoldCollected() const          { return m_unGoldCollected; }
    unsigned int BodyCollected() const          { return m_unBodyCollected; }
    unsigned int RumCollected() const           { return m_unRumCollected; }

    // NOTE: These are INCREMENTAL!!
    void CrateCollected ( unsigned int val )    { m_unCrateCollected += val; }
    void FoodCollected  ( unsigned int val )    { m_unFoodCollected += val; }
    void RumCollected   ( unsigned int val )    { m_unRumCollected += val; }
    void GoldCollected  ( unsigned int val )    { m_unGoldCollected += val; }
    void BodyCollected  ( unsigned int val )    { m_unBodyCollected += val; }

    float TravelDistanceInFoot() const          { return m_fTotalKnot*KNOT_2_FOOT_PER_SEC; }
    unsigned int TotalCannonBallFired() const   { return m_unRegularShotFired + m_unChainShotFired; }
    unsigned int TotalBoarding() const          { return m_unTotalBoarding; }

    // NOTE: These are INCREMENTAL!!
    void TotalKnot( float val ) { m_fTotalKnot += val; }
    void TotalBoarding  ( unsigned int val )    { m_unTotalBoarding += val; }

    void Reset();

    unsigned int CannonUpgraded() const { return m_unCannonUpgraded; }
    void CannonUpgraded( unsigned int val ) { m_unCannonUpgraded = val; }

    unsigned int SailUpgraded() const { return m_unSailUpgraded; }
    void SailUpgraded( unsigned int val ) { m_unSailUpgraded = val; }
    
    unsigned int HullUpgraded() const { return m_unHullUpgraded; }
    void HullUpgraded( unsigned int val ) { m_unHullUpgraded = val; }

private:
    unsigned int    m_unRegularShotFired;
    unsigned int    m_unRegularShotHit;
    unsigned int    m_unChainShotFired;
    unsigned int    m_unChainShotHit;

    unsigned int    m_unFoodShipDestroyed;
    unsigned int    m_unGoldShipDestroyed;
    unsigned int    m_unMilitaryShipDestroyed;

    unsigned int    m_unCrateCollected;
    unsigned int    m_unGoldCollected;
    unsigned int    m_unFoodCollected;
    unsigned int    m_unRumCollected;
    unsigned int    m_unBodyCollected;

    unsigned int    m_unCannonUpgraded;
    unsigned int    m_unSailUpgraded;
    unsigned int    m_unHullUpgraded;

    float           m_fTotalKnot;
    unsigned int    m_unTotalBoarding;

protected:
    GameStatistics();
    ~GameStatistics() = default;
    // Delete stuff for singleton
    GameStatistics( const GameStatistics & ) = delete;
    GameStatistics( GameStatistics&& ) = delete;
    GameStatistics& operator=( GameStatistics const& ) = delete;
    GameStatistics& operator=( GameStatistics && ) = delete;
};

#endif // GameStatics_h__
