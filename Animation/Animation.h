#ifndef Animation_h__
#define Animation_h__

#ifdef ANIMATION_EXPORTS
#define ANIM_API __declspec(dllexport)
#else
#define ANIM_API __declspec(dllimport)
#endif
#include "AnimLayer.h"

namespace FSAnim {
    using namespace DirectX::SimpleMath;

    class CAnimLayer;

    class CAnimation {
    public:
        typedef std::vector<int> BoneStruct_t;
        typedef std::vector<Matrix> BoneOffsets_t;
        typedef std::unordered_map<std::string, CAnimLayer> Animation_t;

        uint32_t    ANIM_API GetBoneCount()const;

        float       ANIM_API GetLayerStartTime( const std::string& LayerName )const;
        float       ANIM_API GetLayerEndTime( const std::string& LayerName )const;
        void        ANIM_API Set( BoneStruct_t& boneHierarchy, BoneOffsets_t& boneOffsets, Animation_t& animations );
        void        ANIM_API GetFinalTransforms( const std::string& LayerName, float timePos, std::vector<Matrix>& finalTransforms, bool flatten = true)const;

    private:

        BoneStruct_t m_boneHierarchy;
        BoneOffsets_t m_boneOffsets;    // the T-Pose
        Animation_t m_animation;
    };
}

#endif // Animation_h__