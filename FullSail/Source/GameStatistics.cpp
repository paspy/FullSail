#include "pch.h"
#include "GameStatistics.h"
#include "GameplayScene.h"

GameStatistics::GameStatistics() :
    m_unRegularShotFired( 0 ),
    m_unRegularShotHit( 0 ),
    m_unChainShotFired( 0 ),
    m_unChainShotHit( 0 ),
    m_unFoodShipDestroyed( 0 ),
    m_unGoldShipDestroyed( 0 ),
    m_unMilitaryShipDestroyed( 0 ),
    m_unCrateCollected( 0 ),
    m_unFoodCollected( 0 ),
    m_unGoldCollected( 0 ),
    m_unRumCollected( 0 ),
    m_unBodyCollected( 0 ),
    m_fTotalKnot( 0.0f ),
    m_unTotalBoarding( 0 ),
    m_unCannonUpgraded( 0 ),
    m_unSailUpgraded( 0 ),
    m_unHullUpgraded( 0 ) {

}
float GameStatistics::RegularShotRate() const {
    float result;
    if ( m_unRegularShotFired == 0 ) {
        return 0.0f;
    }
    result = static_cast<float>( m_unRegularShotHit ) / static_cast<float>( m_unRegularShotFired ) * 100.0f;
    if ( result > 100.0f ) result = 100.0f;
    return result;
}

float GameStatistics::ChainShotRate() const {
    float result;
    if ( m_unChainShotFired == 0 ) {
        return 0.0f;
    }
    result = static_cast<float>( m_unChainShotHit ) / static_cast<float>( m_unChainShotFired )*100.0f;
    if ( result > 100.0f ) result = 100.0f;
    return result;
}

unsigned int GameStatistics::GetFinalScore() const {
    auto cannonScore = ( m_unChainShotHit + m_unRegularShotHit ) * 200;
    auto boardingScore = m_unTotalBoarding * 1000;
    auto collectionScore = m_unBodyCollected * 500 + m_unCrateCollected * 100 + m_unFoodCollected * 10 + m_unRumCollected * 10 + m_unGoldCollected;
    auto smashScore = m_unFoodShipDestroyed * 100 + m_unGoldShipDestroyed * 100 + m_unMilitaryShipDestroyed * 500;
    auto upgradeScore = m_unCannonUpgraded * 200 + m_unSailUpgraded * 200 + m_unHullUpgraded * 200;
    auto travelDeduction = unsigned int( TravelDistanceInFoot()*0.005f );
    int finalScore = int( cannonScore + boardingScore + collectionScore + smashScore + upgradeScore ) - travelDeduction;
    if ( finalScore <= 0 ) finalScore = 0;
    if ( finalScore >= 99999999 ) finalScore = 99999999;
    return unsigned int( finalScore );
}

void GameStatistics::Reset() {
    m_unRegularShotFired = 0;
    m_unRegularShotHit = 0;
    m_unChainShotFired = 0;
    m_unChainShotHit = 0;
    m_unFoodShipDestroyed = 0;
    m_unGoldShipDestroyed = 0;
    m_unMilitaryShipDestroyed = 0;
    m_unCrateCollected = 0;
    m_unFoodCollected = 0;
    m_unRumCollected = 0;
    m_unGoldCollected = 0;
    m_unBodyCollected = 0;
    m_fTotalKnot = 0;
    m_unTotalBoarding = 0;
    m_unCannonUpgraded = 0;
    m_unSailUpgraded = 0;
    m_unHullUpgraded = 0;
}
