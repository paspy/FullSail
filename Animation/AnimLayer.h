#ifndef AnimLayer_h__
#define AnimLayer_h__
#include "AnimBone.h"

namespace FSAnim {

    using namespace DirectX::SimpleMath;
    class CAnimLayer {
        typedef std::vector<CAnimBone> AnimatedBones_t;
    public:

        float GetLayerStartTime() const;
        float GetLayerEndTime() const;

        void Interpolate( float t, std::vector<Matrix>& boneTransforms )const;

        uint32_t Length() const { return m_length; }
        void Length( uint32_t val ) { m_length = val; }

    private:
        AnimatedBones_t m_animatedBones;
        uint32_t m_length;
    };

}
#endif // AnimLayer_h__