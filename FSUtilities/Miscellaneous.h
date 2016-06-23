#ifndef Miscellaneous_h__
#define Miscellaneous_h__
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include <random>

namespace FSUtilities {
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    namespace TerrainLoader {
        struct FileHeader {
            char fileKey[11];
            float version;
        };
        struct TerrainVertex {
            DirectX::XMFLOAT3 Position;
            DirectX::XMFLOAT2 TexCoord;
            DirectX::XMFLOAT2 BoundsY;
        };
    }

    class Misc {
    private:
        static bool AabbBehindPlaneTest( Vector3 center, Vector3 extents, Vector4 plane );
        
    public:

        //static float RandFloat( float a, float b ) { return a + RandFloat()*( b - a ); }
        //static float RandFloat() { return (float)( rand() ) / (float)RAND_MAX; }

        static float RandFloat( float a, float b ) {
            static std::random_device rndDevice;
            std::uniform_real_distribution<float> distribution( a, b );
            return distribution( rndDevice );
        }

        static int RandInt( int a, int b ) {
            static std::random_device rndDevice;
            std::mt19937 rng( rndDevice() );
            std::uniform_int_distribution<int> distribution( a, b );
            return distribution( rng );
        }

        static ID3D11ShaderResourceView* CreateRandomTexture1DSRV( ID3D11Device* device );
        static ID3D11ShaderResourceView* CreateTexture2DArraySRV( ID3D11Device* device, ID3D11DeviceContext* context, std::vector<std::wstring>& filenames );
        static std::vector<ID3D11ShaderResourceView*> CreateTexture2DArraySRV( ID3D11Device * device, std::vector<std::wstring>& filenames );
        static void ExtractFrustumPlanes( XMFLOAT4 planes[6], XMMATRIX M );

        static bool AabbOutsideFrustumTest( Vector3 center, Vector3 extents, Vector4 frustumPlanes[6] );

        static bool LoadFromFSTerrain(
            std::wstring filename,
            ID3D11Device* device,
            float &hmapScale,
            float &hmapResolution,
            float &cellSpacing,
            ID3D11Buffer **vertexBuffer,
            ID3D11Buffer **indexBuffer,
            UINT &numOfIndices,
            ID3D11ShaderResourceView **heightMapSRView,
            std::vector<DirectX::PackedVector::HALF> &hmap );
    };

}
#endif // Miscellaneous_h__
