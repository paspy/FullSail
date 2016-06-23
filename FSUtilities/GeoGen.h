#ifndef GeoGen_h__
#define GeoGen_h__

#include "stdafx.h"

namespace FSUtilities {
    using namespace DirectX;
    using namespace DirectX::SimpleMath;
    namespace GeoGen {
        typedef struct Vertex3D {
            Vertex3D() { Position = Vector3( 0, 0, 0 ); Normal = Vector3( 0, 0, 0 ); TangentU = Vector3( 0, 0, 0 ); TexCoord = Vector2( 0, 0 ); }
            Vertex3D( Vector3 _pos, Vector2 _tex ) : Position( _pos ), TexCoord( _tex ) { }
            Vertex3D( Vector3 _pos, Vector2 _tex, Vector3 _norm ) : Position( _pos ), TexCoord( _tex ), Normal( _norm ) { }
            Vertex3D( const Vector3& p, const Vector3& n, const Vector3& t, const Vector2& uv ) : Position( p ), Normal( n ), TangentU( t ), TexCoord( uv ) { }
            Vertex3D(
                float px, float py, float pz,
                float nx, float ny, float nz,
                float tx, float ty, float tz,
                float u, float v
                ) : Position( px, py, pz ), Normal( nx, ny, nz ), TangentU( tx, ty, tz ), TexCoord( u, v ) { }

            Vector3 Position;
            Vector2 TexCoord;
            Vector3 Normal;
            Vector3 TangentU;

        }*Vertex3D_ptr;

        typedef struct GeoMesh {
            std::vector<Vertex3D> Vertices;
            std::vector<uint32_t> Indices;

        }*GeoMesh_ptr;

        static void CreateBox( float width, float height, float depth, GeoMesh& meshData );
        static void CreateSphere( float radius, UINT sliceCount, UINT stackCount, GeoMesh& meshData );
        static void CreateGeosphere( float radius, UINT numSubdivisions, GeoMesh& meshData );
        static void CreateCylinder( float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, GeoMesh& meshData );
        static void CreateGrid( float width, float depth, UINT m, UINT n, GeoMesh& meshData );

        static void BuildCylinderTopCap( float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, GeoMesh& meshData );
        static void BuildCylinderBottomCap( float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, GeoMesh& meshData );

        static float GetAngleFromXY( float x, float y );
    }
}

#include "GeoGen.hpp"

#endif // GeoGen_h__
