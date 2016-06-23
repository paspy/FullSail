#include "stdafx.h"
#include "Animation.h"

namespace FSAnim {

    uint32_t ANIM_API CAnimation::GetBoneCount() const {
        return static_cast<uint32_t>(m_boneHierarchy.size());
    }

    float ANIM_API CAnimation::GetLayerStartTime( const std::string& LayerName ) const {
        auto layer = m_animation.find( LayerName );
        return layer->second.GetLayerStartTime();
    }

    float ANIM_API CAnimation::GetLayerEndTime( const std::string& LayerName ) const {
        auto layer = m_animation.find( LayerName );
        return layer->second.GetLayerEndTime();
    }

    void ANIM_API CAnimation::Set( BoneStruct_t& boneHierarchy, BoneOffsets_t& boneOffsets, Animation_t& animations ) {
        m_boneHierarchy = boneHierarchy;
        m_boneOffsets = boneOffsets;
        m_animation = animations;
    }

    void ANIM_API CAnimation::GetFinalTransforms( const std::string& LayerName, float timePos, std::vector<Matrix>& finalTransforms, bool flatten) const {

    }

}
