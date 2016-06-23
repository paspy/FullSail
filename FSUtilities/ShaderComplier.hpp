#include "ShaderComplier.h"

namespace FSUtilities {
    namespace SCompiler {
        using namespace std;
        HRESULT CreateShaderAndLayoutFromFile(
            ID3D11Device *_d3dDevice,
            const LPCWSTR _fileName,
            const D3D11_INPUT_ELEMENT_DESC *_inputElemDesc,
            const UINT _elemNum,
            ID3D11VertexShader **_vertexShader,
            ID3D11PixelShader ** _pixelShader,
            ID3D11InputLayout **_inputLayout ) {

            HRESULT hr = E_NOTIMPL;
            ID3DBlob* vertexShaderBlob = nullptr;
            ID3DBlob* pixelShaderBlob = nullptr;
            ID3DBlob* errorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

            hr = D3DCompileFromFile( _fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", flags, 0, &vertexShaderBlob, &errorBlob );

            if ( FAILED( hr ) ) {
                if ( errorBlob ) {
                    string err = (char*)errorBlob->GetBufferPointer();
                    wstring werr( err.begin(), err.end() );
                    MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                    //OutputDebugStringA((char*)errorBlob->GetBufferPointer());
                    SafeRelease( errorBlob );
                }

                SafeRelease( vertexShaderBlob );
                return hr;
            }

            hr = D3DCompileFromFile( _fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", flags, 0, &pixelShaderBlob, &errorBlob );

            if ( FAILED( hr ) ) {
                if ( errorBlob ) {
                    string err = (char*)errorBlob->GetBufferPointer();
                    wstring werr( err.begin(), err.end() );
                    MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                    //OutputDebugStringA((char*)errorBlob->GetBufferPointer());
                    SafeRelease( errorBlob );
                }

                SafeRelease( pixelShaderBlob );
                return hr;
            }

            // create vertex layout
            HR( _d3dDevice->CreateInputLayout( _inputElemDesc, _elemNum, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), _inputLayout ) );
            // create vertex shader
            HR( _d3dDevice->CreateVertexShader( vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, _vertexShader ) );
            // create pixel shader
            HR( _d3dDevice->CreatePixelShader( pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, _pixelShader ) );

            return hr;
        }

        HRESULT CreateOptionalShaderFromFile( ID3D11Device * _d3dDevice, const LPCWSTR _tesselFileName, ID3D11HullShader ** _hullSahder, ID3D11DomainShader ** _domainShader ) {
            HRESULT hr = E_NOTIMPL;
            ID3DBlob* hullShaderBlob = nullptr;
            ID3DBlob* domainShaderBlob = nullptr;
            ID3DBlob* errorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

            hr = D3DCompileFromFile( _tesselFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HSMain", "hs_5_0", flags, 0, &hullShaderBlob, &errorBlob );
            if ( FAILED( hr ) ) {
                if ( errorBlob ) {
                    string err = (char*)errorBlob->GetBufferPointer();
                    wstring werr( err.begin(), err.end() );
                    MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                    SafeRelease( errorBlob );
                }

                SafeRelease( hullShaderBlob );
                return hr;
            }

            hr = D3DCompileFromFile( _tesselFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DSMain", "ds_5_0", flags, 0, &domainShaderBlob, &errorBlob );
            if ( FAILED( hr ) ) {
                if ( errorBlob ) {
                    string err = (char*)errorBlob->GetBufferPointer();
                    wstring werr( err.begin(), err.end() );
                    MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                    SafeRelease( errorBlob );
                }

                SafeRelease( domainShaderBlob );
                return hr;
            }

            // create hull shader
            HR( _d3dDevice->CreateHullShader( hullShaderBlob->GetBufferPointer(), hullShaderBlob->GetBufferSize(), NULL, _hullSahder ) );
            // create domain shader
            HR( _d3dDevice->CreateDomainShader( domainShaderBlob->GetBufferPointer(), domainShaderBlob->GetBufferSize(), NULL, _domainShader ) );

            return hr;
        }

        HRESULT CreateOptionalShaderFromFile(
            ID3D11Device * _d3dDevice,
            const LPCWSTR _geoFileName,
            ID3D11GeometryShader ** _geoShader,
            bool _streamOut,
            ID3D11VertexShader **_streamOutVS,
            D3D11_SO_DECLARATION_ENTRY *_streamOutDecl ) {

            HRESULT hr = E_NOTIMPL;

            ID3DBlob* geoShaderBlob = nullptr;
            ID3DBlob* steamOutVSBlob = nullptr;
            ID3DBlob* errorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

            if ( !_streamOut ) {
                hr = D3DCompileFromFile( _geoFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GSMain", "gs_5_0", flags, 0, &geoShaderBlob, &errorBlob );
                if ( FAILED( hr ) ) {
                    if ( errorBlob ) {
                        string err = (char*)errorBlob->GetBufferPointer();
                        wstring werr( err.begin(), err.end() );
                        MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                        SafeRelease( errorBlob );
                    }

                    SafeRelease( geoShaderBlob );
                    return hr;
                }
                // create geometry shader
                HR( _d3dDevice->CreateGeometryShader( geoShaderBlob->GetBufferPointer(), geoShaderBlob->GetBufferSize(), NULL, _geoShader ) );
            } else {
                if ( _streamOutVS == nullptr || _streamOutDecl == nullptr ) {
                    return E_INVALIDARG;
                }
                hr = D3DCompileFromFile( _geoFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "StreamOutVS", "vs_5_0", flags, 0, &steamOutVSBlob, &errorBlob );

                if ( FAILED( hr ) ) {
                    if ( errorBlob ) {
                        string err = (char*)errorBlob->GetBufferPointer();
                        wstring werr( err.begin(), err.end() );
                        MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                        SafeRelease( errorBlob );
                    }

                    SafeRelease( steamOutVSBlob );
                    return hr;
                }

                hr = D3DCompileFromFile( _geoFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "StreamOutGS", "gs_5_0", flags, 0, &geoShaderBlob, &errorBlob );
                if ( FAILED( hr ) ) {
                    if ( errorBlob ) {
                        string err = (char*)errorBlob->GetBufferPointer();
                        wstring werr( err.begin(), err.end() );
                        MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                        SafeRelease( errorBlob );
                    }

                    SafeRelease( geoShaderBlob );
                    return hr;
                }
                HR( _d3dDevice->CreateVertexShader( steamOutVSBlob->GetBufferPointer(), steamOutVSBlob->GetBufferSize(), NULL, _streamOutVS ) );
                HR( _d3dDevice->CreateGeometryShaderWithStreamOutput( geoShaderBlob->GetBufferPointer(), geoShaderBlob->GetBufferSize(), _streamOutDecl, 5, NULL, 0, 0, NULL, _geoShader ) );
            }

            return hr;
        }

        HRESULT CreateOptionalShaderFromFile( ID3D11Device * _d3dDevice, const LPCWSTR _compFileName, ID3D11ComputeShader ** _compShader, LPCSTR _entryPoint ) {
            HRESULT hr = E_NOTIMPL;

            ID3DBlob* compShaderBlob = nullptr;
            ID3DBlob* errorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

            hr = D3DCompileFromFile( _compFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, _entryPoint, "cs_5_0", flags, 0, &compShaderBlob, &errorBlob );
            if ( FAILED( hr ) ) {
                if ( errorBlob ) {
                    string err = (char*)errorBlob->GetBufferPointer();
                    wstring werr( err.begin(), err.end() );
                    MessageBox( 0, werr.c_str(), L"Error", MB_OK );
                    SafeRelease( errorBlob );
                }

                SafeRelease( compShaderBlob );
                return hr;
            }

            // create geometry shader
            HR( _d3dDevice->CreateComputeShader( compShaderBlob->GetBufferPointer(), compShaderBlob->GetBufferSize(), NULL, _compShader ) );

            return hr;
        }

    }
}