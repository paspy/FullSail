#include "stdafx.h"
#include "AnimLayer.h"

namespace FSAnim {

    float CAnimLayer::GetLayerStartTime() const {
        float t = FLT_MAX;
        for ( auto i = 0; i < m_animatedBones.size(); i++ ) {
            t = min( t, m_animatedBones[i].GetStartTime() );
        }

        return t;
    }

    float CAnimLayer::GetLayerEndTime() const {
        float t = 0.0f;
        for ( auto i = 0; i < m_animatedBones.size(); i++ ) {
            t = max( t, m_animatedBones[i].GetEndTime() );
        }

        return t;
    }

    void CAnimLayer::Interpolate( float t, std::vector<Matrix>& boneTransforms ) const {
        for ( auto i = 0; i < m_animatedBones.size(); i++ ) {
            m_animatedBones[i].Interpolate( t, boneTransforms[i] );
        }
    }

}
