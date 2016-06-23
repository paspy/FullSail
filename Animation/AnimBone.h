#ifndef AnimBone_h__
#define AnimBone_h__

namespace FSAnim {
    using namespace DirectX::SimpleMath;

    struct KeyFrame {
        float Time;
        Matrix Tranform;
    };

    class CAnimBone {
    public:
        typedef std::vector<KeyFrame> Keyframes_t;

        void Interpolate( float t, Matrix& M ) const;

        float GetStartTime() const { return m_keyframes.front().Time; }
        float GetEndTime() const { return m_keyframes.back().Time; }

    private:
        Keyframes_t m_keyframes;

    };
}
#endif // AnimBone_h__

