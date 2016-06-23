#ifndef ShaderComplier_h__
#define ShaderComplier_h__

#include "stdafx.h"

namespace FSUtilities {
    namespace SCompiler {
        static HRESULT CreateShaderAndLayoutFromFile(
            ID3D11Device *_d3dDevice,
            const LPCWSTR _fileName,
            const D3D11_INPUT_ELEMENT_DESC *_inputElemDesc,
            const UINT _elemNum,
            ID3D11VertexShader **_vertexShader,
            ID3D11PixelShader ** _pixelShader,
            ID3D11InputLayout **_inputLayout
            );

        static HRESULT CreateOptionalShaderFromFile(
            ID3D11Device *_d3dDevice,
            const LPCWSTR _tesselFileName,
            ID3D11HullShader **_hullSahder,
            ID3D11DomainShader **_domainShader );

        static HRESULT CreateOptionalShaderFromFile(
            ID3D11Device *_d3dDevice,
            const LPCWSTR _geoFileName,
            ID3D11GeometryShader **_geoShader,
            bool _streamOut = false,
            ID3D11VertexShader **_streamOutVS = nullptr,
            D3D11_SO_DECLARATION_ENTRY *_streamOutDecl = nullptr );

        static HRESULT CreateOptionalShaderFromFile(
            ID3D11Device *_d3dDevice,
            const LPCWSTR _compFileName,
            ID3D11ComputeShader **_compShader,
            LPCSTR _entryPoint = "CSMain" );
    }

}

#include "ShaderComplier.hpp"
#endif // ShaderComplier_h__
