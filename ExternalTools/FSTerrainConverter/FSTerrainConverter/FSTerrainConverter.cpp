//////////////////////////////////////////////////////////////////////////
// Full Sail height map terrain converter
// Author: Chen Lu
// Last Modified by: 04/06/2016
// Version: 0.1
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Core.h"

int wmain( int argc, wchar_t *argv[], wchar_t *envp[] ) {
    if ( argc < 3 ) {
        std::cout << "Usage: " << argv[0] <<" InputFile OutputFile \n";
        return 0;
    }

    std::clock_t tbegin = clock();
    TerrainConv::LoadRawHeightMap( argv[1] );
    TerrainConv::Smooth();
    TerrainConv::CalcAllPatchBoundsY();
    TerrainConv::SaveToFSTerrain( argv[2] );
    std::clock_t tend = clock();

    std::cout << "Time elapsed: " << double( tend - tbegin ) / CLOCKS_PER_SEC << "(s)\n";

    return 0;
}

