#include "stdafx.h"
#include "Core.h"

namespace TerrainConv {

    const int CellsPerPatch = 64;

    float g_heightScale = 25.0f;
    float g_cellSpacing = .25f;
    float g_heightmapResolution = 0;

    UINT g_numPatchVertices;
    UINT g_numPatchQuadFaces;
    UINT g_numPatchVertRows;
    UINT g_numPatchVertCols;

    std::vector<DirectX::XMFLOAT2> g_patchBoundsY;
    std::vector<float> g_heightmap;

    void LoadRawHeightMap( std::wstring filename ) {
        // A height for each vertex
        std::vector<unsigned char> in;

        // Open the file.
        std::ifstream inFile;
        inFile.open( filename, std::ios_base::ate | std::ios_base::binary | std::ios_base::in );
        UINT sizeOfRaw = 0;

        if ( inFile ) {

            sizeOfRaw = (UINT)inFile.tellg();
            g_heightmapResolution = std::sqrt( (float)sizeOfRaw );
            in.resize( sizeOfRaw );

            inFile.seekg( std::ios_base::beg );
            // Read the RAW bytes.
            inFile.read( (char*)&in[0], ( std::streamsize )in.size() );

            // Done with file.
            inFile.close();
        }

        // Copy the array data into a float array and scale it.
        g_heightmap.resize( sizeOfRaw, 0 );
        for ( UINT i = 0; i < sizeOfRaw; ++i ) {
            g_heightmap[i] = ( in[i] / 255.0f )*g_heightScale;
        }

        // Divide height map into patches such that each patch has CellsPerPatch.
        g_numPatchVertRows = ( ( (UINT)g_heightmapResolution - 1 ) / CellsPerPatch ) + 1;
        g_numPatchVertCols = ( ( (UINT)g_heightmapResolution - 1 ) / CellsPerPatch ) + 1;

        g_numPatchVertices = g_numPatchVertRows*g_numPatchVertCols;
        g_numPatchQuadFaces = ( g_numPatchVertRows - 1 )*( g_numPatchVertCols - 1 );

    }

    void Smooth() {
        std::vector<float> dest( g_heightmap.size() );

        for ( UINT i = 0; i < (UINT)g_heightmapResolution; ++i ) {
            for ( UINT j = 0; j < (UINT)g_heightmapResolution; ++j ) {
                dest[i*(UINT)g_heightmapResolution + j] = Average( i, j );
            }
        }

        // Replace the old height map with the filtered one.
        g_heightmap = dest;
    }

    bool InBounds( int i, int j ) {
        // True if ij are valid indices; false otherwise.
        return
            i >= 0 && i < (int)g_heightmapResolution &&
            j >= 0 && j < (int)g_heightmapResolution;
    }

    float Average( int i, int j ) {
        // Function computes the average height of the ij element.
        // It averages itself with its eight neighbor pixels.  Note
        // that if a pixel is missing neighbor, we just don't include it
        // in the average--that is, edge pixels don't have a neighbor pixel.
        //
        // ----------
        // | 1| 2| 3|
        // ----------
        // |4 |ij| 6|
        // ----------
        // | 7| 8| 9|
        // ----------

        float avg = 0.0f;
        float num = 0.0f;

        // Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
        // and no iterations of the outer for loop occur.
        for ( int m = i - 1; m <= i + 1; ++m ) {
            for ( int n = j - 1; n <= j + 1; ++n ) {
                if ( InBounds( m, n ) ) {
                    avg += g_heightmap[m*(UINT)g_heightmapResolution + n];
                    num += 1.0f;
                }
            }
        }

        return avg / num;
    }

    void CalcAllPatchBoundsY() {
        g_patchBoundsY.resize( g_numPatchQuadFaces );

        // For each patch
        for ( UINT i = 0; i < g_numPatchVertRows - 1; ++i ) {
            for ( UINT j = 0; j < g_numPatchVertCols - 1; ++j ) {
                CalcPatchBoundsY( i, j );
            }
        }
    }

    void CalcPatchBoundsY( UINT i, UINT j ) {
        // Scan the height map values this patch covers and compute the min/max height.

        UINT x0 = j*CellsPerPatch;
        UINT x1 = ( j + 1 )*CellsPerPatch;

        UINT y0 = i*CellsPerPatch;
        UINT y1 = ( i + 1 )*CellsPerPatch;

        float minY = +FLT_MAX;
        float maxY = -FLT_MAX;
        for ( UINT y = y0; y <= y1; ++y ) {
            for ( UINT x = x0; x <= x1; ++x ) {
                UINT k = y*(UINT)g_heightmapResolution + x;
                minY = min( minY, g_heightmap[k] );
                maxY = max( maxY, g_heightmap[k] );
            }
        }

        UINT patchID = i*( g_numPatchVertCols - 1 ) + j;
        g_patchBoundsY[patchID] = DirectX::XMFLOAT2( minY, maxY );
    }

    float GetWidth() {
        // Total terrain width.
        return ( g_heightmapResolution - 1 )*g_cellSpacing;
    }

    float GetDepth() {
        // Total terrain depth.
        return ( g_heightmapResolution - 1 )*g_cellSpacing;
    }

    void SaveToFSTerrain( std::wstring filename ) {

        std::vector<VertexT> patchVertices( g_numPatchVertRows*g_numPatchVertCols );
        UINT vertexByteWidth = 0;
        std::vector<USHORT> indices( g_numPatchQuadFaces * 4 ); // 4 indices per quad face
        UINT indexByteWidth = 0;

        std::vector<DirectX::PackedVector::HALF> hmap( g_heightmap.size() );
        std::transform( g_heightmap.begin(), g_heightmap.end(), hmap.begin(), DirectX::PackedVector::XMConvertFloatToHalf );
        UINT hmapDataPitch = (UINT)g_heightmapResolution * sizeof( DirectX::PackedVector::HALF );

#pragma region cacluate_patch_verts
        float halfWidth = 0.5f*GetWidth();
        float halfDepth = 0.5f*GetDepth();

        float patchWidth = GetWidth() / ( g_numPatchVertCols - 1 );
        float patchDepth = GetDepth() / ( g_numPatchVertRows - 1 );
        float du = 1.0f / ( g_numPatchVertCols - 1 );
        float dv = 1.0f / ( g_numPatchVertRows - 1 );

        for ( UINT i = 0; i < g_numPatchVertRows; ++i ) {
            float z = halfDepth - i*patchDepth;
            for ( UINT j = 0; j < g_numPatchVertCols; ++j ) {
                float x = -halfWidth + j*patchWidth;

                patchVertices[i*g_numPatchVertCols + j].Position = DirectX::XMFLOAT3( x, 0.0f, z );

                // Stretch texture over grid.
                patchVertices[i*g_numPatchVertCols + j].TexCoord.x = j*du;
                patchVertices[i*g_numPatchVertCols + j].TexCoord.y = i*dv;
            }
        }

        // Store axis-aligned bounding box y-bounds in upper-left patch corner.
        for ( UINT i = 0; i < g_numPatchVertRows - 1; ++i ) {
            for ( UINT j = 0; j < g_numPatchVertCols - 1; ++j ) {
                UINT patchID = i*( g_numPatchVertCols - 1 ) + j;
                patchVertices[i*g_numPatchVertCols + j].BoundsY = g_patchBoundsY[patchID];
            }
        }
#pragma endregion cacluate_patch_verts

#pragma region cacluate_indces
        // Iterate over each quad and compute indices.
        int k = 0;
        for ( UINT i = 0; i < g_numPatchVertRows - 1; ++i ) {
            for ( UINT j = 0; j < g_numPatchVertCols - 1; ++j ) {
                // Top row of 2x2 quad patch
                indices[k] = i*g_numPatchVertCols + j;
                indices[k + 1] = i*g_numPatchVertCols + j + 1;

                // Bottom row of 2x2 quad patch
                indices[k + 2] = ( i + 1 )*g_numPatchVertCols + j;
                indices[k + 3] = ( i + 1 )*g_numPatchVertCols + j + 1;

                k += 4; // next quad
            }
        }
#pragma endregion cacluate_indces

        vertexByteWidth = sizeof( VertexT ) * static_cast<UINT>( patchVertices.size() );
        indexByteWidth = sizeof( USHORT ) * static_cast<UINT>( indices.size() );

        // File Header
        std::ofstream outFile;

        outFile.open( filename.c_str(), std::ios_base::binary | std::ios_base::out );

        if ( outFile ) {
            FileHeader fileHeader;
            unsigned int sizeOfDate = 0;
            strcpy_s( fileHeader.fileKey, 11, _FILE_HEDAER );
            fileHeader.version = _VERSION;

            // write header
            outFile.write( (char*)&fileHeader, sizeof( FileHeader ) );

            // write terrain info. - scale, cell spacing and resolution
            outFile.write( (char*)&g_heightScale, sizeof( float ) );
            outFile.write( (char*)&g_cellSpacing, sizeof( float ) );
            outFile.write( (char*)&g_heightmapResolution, sizeof( float ) );

            // write vertices and byte width
            sizeOfDate = (unsigned int)patchVertices.size();
            outFile.write( (char*)&vertexByteWidth, sizeof( UINT ) );
            outFile.write( (char*)&sizeOfDate, sizeof( unsigned int ) );
            outFile.write( (char*)patchVertices.data(), sizeof( VertexT )*sizeOfDate );

            // write indices and byte width
            sizeOfDate = (unsigned int)indices.size();
            outFile.write( (char*)&indexByteWidth, sizeof( UINT ) );
            outFile.write( (char*)&sizeOfDate, sizeof( unsigned int ) );
            outFile.write( (char*)indices.data(), sizeof( USHORT )*sizeOfDate );

            // write height map for SRV
            sizeOfDate = (unsigned int)hmap.size();
            outFile.write( (char*)&hmapDataPitch, sizeof( UINT ) );
            outFile.write( (char*)&sizeOfDate, sizeof( unsigned int ) );
            outFile.write( (char*)hmap.data(), sizeof( DirectX::PackedVector::HALF )*sizeOfDate );

            outFile.close();
        }
    }
}