#include "stdafx.h"
#include "AnimBone.h"


namespace FSAnim {

    void CAnimBone::Interpolate( float t, Matrix & M ) const {
        if ( t <= GetStartTime() ) {
            M = m_keyframes.front().Tranform;

        } else if ( t >= GetEndTime() ) {
            M = m_keyframes.back().Tranform;

        } else {
            for ( auto i = 0; i < m_keyframes.size() - 1; i++ ) {
                if ( t >= m_keyframes[i].Time && t <= m_keyframes[i + 1].Time ) {
                    M = Matrix::Lerp( 
                        m_keyframes[i].Tranform,
                        m_keyframes[i + 1].Tranform,
                        t 
                    );
                }
            }

        }

    }

}
