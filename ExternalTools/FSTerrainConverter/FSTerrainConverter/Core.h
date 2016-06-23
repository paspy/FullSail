#pragma once
#include "stdafx.h"

namespace TerrainConv {

    struct VertexT {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT2 TexCoord;
        DirectX::XMFLOAT2 BoundsY;
    };

    struct FileHeader {
        char fileKey[11];
        float version;
    };

    extern const int CellsPerPatch;

    extern float g_heightScale;
    extern float g_cellSpacing;
    extern float g_heightmapResolution;

    extern UINT g_numPatchVertices;
    extern UINT g_numPatchQuadFaces;
    extern UINT g_numPatchVertRows;
    extern UINT g_numPatchVertCols;

    extern std::vector<DirectX::XMFLOAT2> g_patchBoundsY;
    extern std::vector<float> g_heightmap;

    void LoadRawHeightMap( std::wstring filename );
    void Smooth();
    bool InBounds( int i, int j );
    float Average( int i, int j );
    void CalcAllPatchBoundsY();
    void CalcPatchBoundsY( UINT i, UINT j );
    float GetWidth();
    float GetDepth();

    void SaveToFSTerrain( std::wstring filename );

}