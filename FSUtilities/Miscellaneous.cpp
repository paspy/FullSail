#include "stdafx.h"
#include "Miscellaneous.h"
#include <MemoryManager.h>

namespace FSUtilities {

    using namespace Microsoft::WRL;

    bool Misc::AabbBehindPlaneTest( Vector3 center, Vector3 extents, Vector4 plane ) {
        XMVECTOR n = XMVectorAbs( XMLoadFloat4( &plane ) );
        float r = XMVectorGetX( XMVector3Dot( XMLoadFloat3( &extents ), n ) );
        float s = XMVectorGetX( XMVector3Dot(
            XMLoadFloat4( &Vector4( center.x, center.y, center.z, 1.0f ) ), XMLoadFloat4( &plane )
        ) );
        return ( ( s + r ) < 0.0f );
    }

    ID3D11ShaderResourceView * Misc::CreateRandomTexture1DSRV( ID3D11Device * device ) {
        // Create the random data.
        XMFLOAT4 *randomValues;

        randomValues = reinterpret_cast<XMFLOAT4*>( FSMM::MemoryManager::GetInstance()->Allocate( sizeof( XMFLOAT4 ) * 1024, 0U, __FILE__, __LINE__ ) );

        for ( int i = 0; i < 1024; ++i ) {
            randomValues[i].x = RandFloat( -1.0f, 1.0f );
            randomValues[i].y = RandFloat( -1.0f, 1.0f );
            randomValues[i].z = RandFloat( -1.0f, 1.0f );
            randomValues[i].w = RandFloat( -1.0f, 1.0f );
        }

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = randomValues;
        initData.SysMemPitch = 1024 * sizeof( XMFLOAT4 );
        initData.SysMemSlicePitch = 0;

        //
        // Create the texture.
        //
        D3D11_TEXTURE1D_DESC texDesc;
        texDesc.Width = 1024;
        texDesc.MipLevels = 1;
        texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        texDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = 0;
        texDesc.ArraySize = 1;

        ComPtr<ID3D11Texture1D> randomTex;

        HR( device->CreateTexture1D( &texDesc, &initData, randomTex.GetAddressOf() ) );

        FSMM::MemoryManager::GetInstance()->DeAllocate( randomValues );

        //
        // Create the resource view.
        //
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = texDesc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
        viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
        viewDesc.Texture1D.MostDetailedMip = 0;

        ID3D11ShaderResourceView* randomTexSRV = nullptr;
        HR( device->CreateShaderResourceView( randomTex.Get(), &viewDesc, &randomTexSRV ) );

        return randomTexSRV;
    }

    ID3D11ShaderResourceView* Misc::CreateTexture2DArraySRV( ID3D11Device* device, ID3D11DeviceContext* context, std::vector<std::wstring>& filenames ) {
        auto size = filenames.size();

        std::vector<ComPtr<ID3D11Texture2D>> srcTex( size );

        for ( auto i = 0; i < size; ++i ) {
            HR( CreateDDSTextureFromFileEx(
                device, context, filenames[i].c_str(), NULL, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, false, (ID3D11Resource**)srcTex[i].GetAddressOf(), NULL )
            );
        }

        D3D11_TEXTURE2D_DESC texElementDesc;
        srcTex[0].Get()->GetDesc( &texElementDesc );

        D3D11_TEXTURE2D_DESC texArrayDesc;
        texArrayDesc.Width = texElementDesc.Width;
        texArrayDesc.Height = texElementDesc.Height;
        texArrayDesc.MipLevels = texElementDesc.MipLevels;
        texArrayDesc.ArraySize = (UINT)size;
        texArrayDesc.Format = texElementDesc.Format;
        texArrayDesc.SampleDesc.Count = 1;
        texArrayDesc.SampleDesc.Quality = 0;
        texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
        texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texArrayDesc.CPUAccessFlags = 0;
        texArrayDesc.MiscFlags = 0;

        //ID3D11Texture2D* texArray = 0;
        ComPtr<ID3D11Texture2D> texArray;
        HR( device->CreateTexture2D( &texArrayDesc, 0, texArray.GetAddressOf() ) );

        // for each texture element...
        for ( UINT texElement = 0; texElement < size; ++texElement ) {
            // for each mip level...
            for ( UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel ) {
                D3D11_MAPPED_SUBRESOURCE mappedTex2D;
                HR( context->Map( srcTex[texElement].Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D ) );

                context->UpdateSubresource(
                    texArray.Get(), D3D11CalcSubresource( mipLevel, texElement, texElementDesc.MipLevels ),
                    0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch
                );

                context->Unmap( srcTex[texElement].Get(), mipLevel );
            }
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.Format = texArrayDesc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.MostDetailedMip = 0;
        viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
        viewDesc.Texture2DArray.FirstArraySlice = 0;
        viewDesc.Texture2DArray.ArraySize = (UINT)size;

        ID3D11ShaderResourceView* texArraySRV = nullptr;
        HR( device->CreateShaderResourceView( texArray.Get(), &viewDesc, &texArraySRV ) );

        return texArraySRV;
    }

    std::vector<ID3D11ShaderResourceView*> Misc::CreateTexture2DArraySRV( ID3D11Device * device, std::vector<std::wstring>& filenames ) {
        UINT size = static_cast<UINT>( filenames.size() );

        std::vector<ID3D11ShaderResourceView*> srcTex( size );
        for ( UINT i = 0; i < size; ++i ) {
            HR( CreateDDSTextureFromFile( device, filenames[i].c_str(), NULL, &srcTex[i] ) );
        }

        return srcTex;
    }

    void Misc::ExtractFrustumPlanes( XMFLOAT4 planes[6], XMMATRIX M ) {
        // Left
        planes[0].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[0];
        planes[0].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[0];
        planes[0].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[0];
        planes[0].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[0];

        // Right
        planes[1].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[0];
        planes[1].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[0];
        planes[1].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[0];
        planes[1].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[0];

        // Bottom
        planes[2].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[1];
        planes[2].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[1];
        planes[2].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[1];
        planes[2].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[1];

        // Top
        planes[3].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[1];
        planes[3].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[1];
        planes[3].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[1];
        planes[3].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[1];

        // Near
        planes[4].x = M.r[0].m128_f32[2];
        planes[4].y = M.r[1].m128_f32[2];
        planes[4].z = M.r[2].m128_f32[2];
        planes[4].w = M.r[3].m128_f32[2];

        // Far
        planes[5].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[2];
        planes[5].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[2];
        planes[5].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[2];
        planes[5].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[2];

        // Normalize the plane equations.
        for ( int i = 0; i < 6; ++i ) {
            XMVECTOR v = XMPlaneNormalize( XMLoadFloat4( &planes[i] ) );
            XMStoreFloat4( &planes[i], v );
        }
    }

    bool Misc::AabbOutsideFrustumTest( Vector3 center, Vector3 extents, Vector4 frustumPlanes[6] ) {
        for ( int i = 0; i < 6; ++i ) {
            if ( AabbBehindPlaneTest( center, extents, frustumPlanes[i] ) ) {
                return true;
            }
        }

        return false;
    }

    bool Misc::LoadFromFSTerrain(
        std::wstring filename,
        ID3D11Device* device,
        float &hmapScale,
        float &hmapResolution,
        float &cellSpacing,
        ID3D11Buffer **vertexBuffer,
        ID3D11Buffer **indexBuffer,
        UINT &numOfIndices,
        ID3D11ShaderResourceView **heightMapSRView,
        std::vector<DirectX::PackedVector::HALF> &hmap ) {

        std::ifstream inFile;
        inFile.open( filename.c_str(), std::ios_base::binary | std::ios_base::in );

        if ( inFile ) {
            unsigned int sizeOfDate = 0;
            TerrainLoader::FileHeader fileHeader;
            inFile.read( (char*)&fileHeader, sizeof( TerrainLoader::FileHeader ) );
            // *skip verification* //

            // read terrain info. - scale, cell spacing and resolution
            inFile.read( (char*)&hmapScale, sizeof( float ) );
            inFile.read( (char*)&cellSpacing, sizeof( float ) );
            inFile.read( (char*)&hmapResolution, sizeof( float ) );

            // read vertices and byte width
            UINT vertexByteWidth = 0;
            inFile.read( (char*)&vertexByteWidth, sizeof( UINT ) );
            inFile.read( (char*)&sizeOfDate, sizeof( unsigned int ) );
            std::vector<TerrainLoader::TerrainVertex> patchVertices( sizeOfDate );
            inFile.read( (char*)patchVertices.data(), sizeof( TerrainLoader::TerrainVertex )*sizeOfDate );

            // read indices and byte width
            UINT indexByteWidth = 0;
            inFile.read( (char*)&indexByteWidth, sizeof( UINT ) );
            inFile.read( (char*)&sizeOfDate, sizeof( unsigned int ) );
            numOfIndices = sizeOfDate;
            std::vector<USHORT> indices( sizeOfDate );
            inFile.read( (char*)indices.data(), sizeof( USHORT )*sizeOfDate );

            // read height map for SRV
            UINT hmapDataPitch = 0;
            inFile.read( (char*)&hmapDataPitch, sizeof( UINT ) );
            inFile.read( (char*)&sizeOfDate, sizeof( unsigned int ) );
            std::vector<DirectX::PackedVector::HALF> hmapTmp( sizeOfDate );
            inFile.read( (char*)hmapTmp.data(), sizeof( DirectX::PackedVector::HALF )*sizeOfDate );

            D3D11_BUFFER_DESC vbd;
            SecureZeroMemory( &vbd, sizeof( vbd ) );
            vbd.Usage = D3D11_USAGE_IMMUTABLE;
            vbd.ByteWidth = sizeof( TerrainLoader::TerrainVertex ) * static_cast<UINT>( patchVertices.size() );
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vinitData;
            vinitData.pSysMem = &patchVertices[0];
            HR( device->CreateBuffer( &vbd, &vinitData, vertexBuffer ) );

            D3D11_BUFFER_DESC ibd;
            SecureZeroMemory( &ibd, sizeof( ibd ) );
            ibd.Usage = D3D11_USAGE_IMMUTABLE;
            ibd.ByteWidth = sizeof( USHORT ) * static_cast<UINT>( indices.size() );
            ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA iinitData;
            iinitData.pSysMem = &indices[0];
            HR( device->CreateBuffer( &ibd, &iinitData, indexBuffer ) );

            D3D11_TEXTURE2D_DESC texDesc;
            SecureZeroMemory( &texDesc, sizeof( texDesc ) );
            texDesc.Width = (UINT)hmapResolution;
            texDesc.Height = (UINT)hmapResolution;
            texDesc.MipLevels = 1;
            texDesc.ArraySize = 1;
            texDesc.Format = DXGI_FORMAT_R16_FLOAT;
            texDesc.SampleDesc.Count = 1;
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA sdata;
            SecureZeroMemory( &sdata, sizeof( sdata ) );
            sdata.pSysMem = &hmapTmp[0];
            sdata.SysMemPitch = (UINT)hmapResolution * sizeof( DirectX::PackedVector::HALF );

            ID3D11Texture2D* hmapTex = nullptr;
            HR( device->CreateTexture2D( &texDesc, &sdata, &hmapTex ) );

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            SecureZeroMemory( &srvDesc, sizeof( srvDesc ) );
            srvDesc.Format = texDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = -1;
            HR( device->CreateShaderResourceView( hmapTex, &srvDesc, heightMapSRView ) );

            hmap = hmapTmp;

            SafeRelease( hmapTex );
            inFile.close();
            return true;
        }
        return false;
    }
}