#pragma once
#include "pch.h"

/** Helper structs for quickly creating render-to-texture buffers */

/** Struct for a texture that can be used as shader resource AND rendertarget */
struct RenderToTextureBuffer {
    ~RenderToTextureBuffer() {
    }

    /** Creates the render-to-texture buffers */
    RenderToTextureBuffer( const Microsoft::WRL::ComPtr<ID3D11Device1>& device, UINT SizeX, UINT SizeY, DXGI_FORMAT Format, HRESULT* Result = nullptr, DXGI_FORMAT RTVFormat = DXGI_FORMAT_UNKNOWN, DXGI_FORMAT SRVFormat = DXGI_FORMAT_UNKNOWN, int MipLevels = 1, UINT arraySize = 1 ) {
        HRESULT hr = S_OK;

        ZeroMemory( CubeMapRTVs, sizeof( CubeMapRTVs ) );

        if ( SizeX == 0 || SizeY == 0 ) {
            LogError() << L"SizeX or SizeY can't be 0";
        }

        this->SizeX = SizeX;
        this->SizeY = SizeY;

        if ( Format == 0 ) {
            LogError() << L"DXGI_FORMAT_UNKNOWN (0) isn't a valid texture format";
        }

        //Create a new render target texture
        D3D11_TEXTURE2D_DESC Desc = CD3D11_TEXTURE2D_DESC(
            Format,
            SizeX,
            SizeY,
            arraySize,
            MipLevels,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );

        if ( arraySize > 1 )
            Desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        if ( MipLevels != 1 )
            Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        LE( device->CreateTexture2D( &Desc, nullptr, Texture.GetAddressOf() ) );

        // Can't do further work if texture is null.
        if ( !Texture.Get() ) return;

        //Create a render target view
        D3D11_RENDER_TARGET_VIEW_DESC DescRT = CD3D11_RENDER_TARGET_VIEW_DESC();
        DescRT.Format = (RTVFormat != DXGI_FORMAT_UNKNOWN ? RTVFormat : Desc.Format);
        DescRT.Texture2D.MipSlice = 0;
        DescRT.Texture2DArray.ArraySize = arraySize;

        if ( arraySize == 1 )
            DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        else {
            DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            DescRT.Texture2DArray.FirstArraySlice = 0;
        }

        LE( device->CreateRenderTargetView( Texture.Get(), &DescRT, RenderTargetView.GetAddressOf() ) );

        if ( arraySize > 1 ) {
            // Create the one-face render target views
            DescRT.Texture2DArray.ArraySize = 1;
            for ( int i = 0; i < 6; ++i ) {
                DescRT.Texture2DArray.FirstArraySlice = i;
                LE( device->CreateRenderTargetView( Texture.Get(), &DescRT, CubeMapRTVs[i].GetAddressOf() ) );
            }
        }

        // Create the resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = CD3D11_SHADER_RESOURCE_VIEW_DESC();
        DescRV.Format = (SRVFormat != DXGI_FORMAT_UNKNOWN ? SRVFormat : Desc.Format);

        if ( arraySize > 1 )
            DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        else
            DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        DescRV.Texture2D.MipLevels = MipLevels;
        DescRV.Texture2D.MostDetailedMip = 0;

        LE( device->CreateShaderResourceView( Texture.Get(), &DescRV, ShaderResView.GetAddressOf() ) );

        if ( FAILED( hr ) ) {
            LogError() << L"Coould not create ID3D11Texture2D, ID3D11ShaderResourceView, or ID3D11RenderTargetView. Killing created resources (If any).";
            ReleaseAll();
            if ( Result )*Result = hr;
            return;
        }

        //LogInfo() << L"Successfully created ID3D11Texture2D, ID3D11ShaderResourceView, and ID3D11RenderTargetView.";
        if ( Result )*Result = hr;
    }

    /** Binds the texture to the pixel shader */
    void BindToPixelShader( const Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& context, int slot ) {
        context->PSSetShaderResources( slot, 1, ShaderResView.GetAddressOf() );
    };

    const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture() { return Texture; }
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResView() { return ShaderResView; }
    const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return RenderTargetView; }

    //void SetTexture( ID3D11Texture2D* tx ) { Texture = tx; }
    //void SetShaderResView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv ) { ShaderResView = srv.Get(); }
    //void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv ) { RenderTargetView = rtv.Get(); }

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTVCubemapFace( UINT i ) { return CubeMapRTVs[i]; }

    UINT GetSizeX() { return SizeX; }
    UINT GetSizeY() { return SizeY; }
private:

    /** The Texture object */
    Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;

    /** Shader and rendertarget resource views */
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

    // Rendertargets for the cubemap-faces, if this is a cubemap
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> CubeMapRTVs[6];

    UINT SizeX;
    UINT SizeY;

    void ReleaseAll() {
        Texture.Reset();
        ShaderResView.Reset();
        RenderTargetView.Reset();
    }
};

/** Struct for a texture that can be used as shader resource AND depth stencil target */
struct RenderToDepthStencilBuffer {
    ~RenderToDepthStencilBuffer() {
    }

    /** Creates the render-to-texture buffers */
    RenderToDepthStencilBuffer( const Microsoft::WRL::ComPtr<ID3D11Device1>& device, UINT SizeX, UINT SizeY, DXGI_FORMAT Format, HRESULT* Result = nullptr, DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN, DXGI_FORMAT SRVFormat = DXGI_FORMAT_UNKNOWN, UINT arraySize = 1 ) {
        HRESULT hr = S_OK;

        if ( arraySize != 1 && arraySize != 6 ) {
            LogError() << "Only supporting single render targets and cubemaps ATM. Unsupported Arraysize: " << arraySize;
            return;
        }

        if ( SizeX == 0 || SizeY == 0 ) {
            LogError() << L"SizeX or SizeY can't be 0";
        }

        this->SizeX = SizeX;
        this->SizeY = SizeY;

        if ( Format == 0 ) {
            LogError() << L"DXGI_FORMAT_UNKNOWN (0) isn't a valid texture format";
        }

        //Create a new render target texture
        D3D11_TEXTURE2D_DESC Desc = CD3D11_TEXTURE2D_DESC(
            Format,
            SizeX,
            SizeY,
            arraySize,
            1,
            D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE );

        if ( arraySize > 1 )
            Desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        LE( device->CreateTexture2D( &Desc, nullptr, Texture.GetAddressOf() ) );

        if ( !Texture.Get() ) {
            LogError() << "Could not create Texture!";
            return;
        }

        //Create a render target view
        D3D11_DEPTH_STENCIL_VIEW_DESC DescDSV = CD3D11_DEPTH_STENCIL_VIEW_DESC();
        ZeroMemory( &DescDSV, sizeof( DescDSV ) );
        DescDSV.Format = (DSVFormat != DXGI_FORMAT_UNKNOWN ? DSVFormat : Desc.Format);

        if ( arraySize == 1 )
            DescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        else {
            DescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            DescDSV.Texture2DArray.FirstArraySlice = 0;
            DescDSV.Texture2DArray.ArraySize = arraySize;
        }

        DescDSV.Texture2D.MipSlice = 0;
        DescDSV.Flags = 0;

        LE( device->CreateDepthStencilView( Texture.Get(), &DescDSV, DepthStencilView.GetAddressOf() ) );

        if ( arraySize > 1 ) {
            // Create the one-face render target views
            DescDSV.Texture2DArray.ArraySize = 1;
            for ( int i = 0; i < 6; ++i ) {
                DescDSV.Texture2DArray.FirstArraySlice = i;
                LE( device->CreateDepthStencilView( Texture.Get(), &DescDSV, CubeMapDSVs[i].GetAddressOf() ) );
            }
        }

        // Create the resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = CD3D11_SHADER_RESOURCE_VIEW_DESC();
        DescRV.Format = (SRVFormat != DXGI_FORMAT_UNKNOWN ? SRVFormat : Desc.Format);
        if ( arraySize > 1 )
            DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        else
            DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        DescRV.Texture2D.MipLevels = 1;
        DescRV.Texture2D.MostDetailedMip = 0;

        LE( device->CreateShaderResourceView( Texture.Get(), &DescRV, ShaderResView.GetAddressOf() ) );

        if ( FAILED( hr ) ) {
            LogError() << L"Could not create ID3D11Texture2D, ID3D11ShaderResourceView, or ID3D11DepthStencilView. Killing created resources (If any).";
            if ( Result )*Result = hr;
            return;
        }


        //LogInfo() << L"RenderToDepthStencilStruct: Successfully created ID3D11Texture2D, ID3D11ShaderResourceView, and ID3D11DepthStencilView.";
        if ( Result )*Result = hr;
    }

    void BindToVertexShader( const Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& context, int slot ) {
        context->VSSetShaderResources( slot, 1, ShaderResView.GetAddressOf() );
    }

    void BindToPixelShader( const Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& context, int slot ) {
        context->PSSetShaderResources( slot, 1, ShaderResView.GetAddressOf() );
    }

    const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture() const { return Texture; }
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResView() const { return ShaderResView; }
    const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const { return DepthStencilView; }
    UINT GetSizeX() const { return SizeX; }
    UINT GetSizeY() const { return SizeY; }

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDSVCubemapFace( UINT i ) { return CubeMapDSVs[i].Get(); }

    //void SetTexture( Microsoft::WRL::ComPtr<ID3D11Texture2D> tx ) { Texture = tx.Get(); }
    //void SetShaderResView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv ) { ShaderResView = srv.Get(); }
    //void SetDepthStencilView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv ) { DepthStencilView = dsv.Get(); }

private:

    // The Texture object
    Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;

    UINT SizeX;
    UINT SizeY;

    // Shader and rendertarget resource views
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

    // Rendertargets for the cubemap-faces, if this is a cubemap
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> CubeMapDSVs[6];
};
