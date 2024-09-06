#pragma once
#include "d3d11pfx_effect.h"

class D3D11PFX_HeightFog :
    public D3D11PFX_Effect {
public:
    D3D11PFX_HeightFog( D3D11PfxRenderer* rnd );
    ~D3D11PFX_HeightFog();

    /** Draws this effect to the given buffer */
    XRESULT Render( RenderToTextureBuffer* fxbuffer );
};

