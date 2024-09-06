#pragma once
#include "pch.h"

class GFSDK_SSAO_Context_D3D11;
class D3D11NVHBAO {
public:
    D3D11NVHBAO();
    ~D3D11NVHBAO();

    /** Initializes the library */
    XRESULT Init();

    /** Renders the HBAO-Effect onto the given RTV */
    XRESULT Render( Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pOutputColorRTV );
private:
    /** Nvidia HBAO+ context */
    GFSDK_SSAO_Context_D3D11* AOContext;
};

