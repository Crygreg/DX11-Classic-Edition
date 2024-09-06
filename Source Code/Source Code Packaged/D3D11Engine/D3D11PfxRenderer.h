#pragma once
#include "pch.h"

struct RenderToTextureBuffer;
class D3D11PFX_Blur;
class D3D11PFX_HeightFog;
class D3D11PFX_DistanceBlur;
class D3D11PFX_HDR;
class D3D11PFX_SMAA;
class D3D11PFX_GodRays;
class D3D11NVHBAO;
class D3D11PfxRenderer {
public:
    D3D11PfxRenderer();
    ~D3D11PfxRenderer();

    /** Called on resize */
    XRESULT OnResize( const INT2& newResolution );

    /** Blurs the given texture */
    XRESULT BlurTexture( RenderToTextureBuffer* texture, bool leaveResultInD4_2 = false, float scale = 1.0f, const XMFLOAT4& colorMod = XMFLOAT4( 1, 1, 1, 1 ), const std::string& finalCopyShader = "PS_PFX_Simple" );

    /** Renders the heightfog */
    XRESULT RenderHeightfog();

    /** Renders the distance blur effect */
    XRESULT RenderDistanceBlur();

    /** Renders the HDR-Effect */
    XRESULT RenderHDR();

    /** Renders the SMAA-Effect */
    XRESULT RenderSMAA();

    /** Renders the godrays-Effect */
    XRESULT RenderGodRays();

    /** Copies the given texture to the given RTV */
    XRESULT CopyTextureToRTV( const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture, const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, INT2 targetResolution = INT2( 0, 0 ), bool useCustomPS = false, INT2 offset = INT2( 0, 0 ) );

    /** Unbinds texturesamplers from the pixel-shader */
    XRESULT UnbindPSResources( int num );

    /** Draws a fullscreenquad */
    XRESULT DrawFullScreenQuad();

    /** Draws the HBAO-Effect to the given buffer */
    XRESULT DrawHBAO( const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv );

    /** Accessors */
    RenderToTextureBuffer& GetTempBuffer() { return *TempBuffer; }
    RenderToTextureBuffer& GetTempBufferDS4_1() { return *TempBufferDS4_1; }
    RenderToTextureBuffer& GetTempBufferDS4_2() { return *TempBufferDS4_2; }

private:
    /** Temporary buffer in the same size/format as the backbuffer */
    std::unique_ptr<RenderToTextureBuffer> TempBuffer;

    /** Temporary buffer with the resolution divided by 4 */
    std::unique_ptr<RenderToTextureBuffer> TempBufferDS4_1;
    std::unique_ptr<RenderToTextureBuffer> TempBufferDS4_2;

    /** Blur effect referenced here because it's often needed by PFX */
    std::unique_ptr<D3D11PFX_Blur> FX_Blur;
    std::unique_ptr<D3D11PFX_HeightFog> FX_HeightFog;
    std::unique_ptr<D3D11PFX_DistanceBlur> FX_DistanceBlur;
    std::unique_ptr<D3D11PFX_HDR> FX_HDR;
    std::unique_ptr<D3D11PFX_SMAA> FX_SMAA;
    std::unique_ptr<D3D11PFX_GodRays> FX_GodRays;

    /** Nivida HBAO+ */
    std::unique_ptr<D3D11NVHBAO> NvHBAO;
};

