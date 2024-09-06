//--------------------------------------------------------------------------------------
// Simple vertex shader
//--------------------------------------------------------------------------------------

cbuffer Viewport : register( b0 )
{
	float2 V_ViewportPos;
	float2 V_ViewportSize;
};


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTex1		: TEXCOORD0;
	float2 vTex2		: TEXCOORD1;
	float4 vDiffuse		: DIFFUSE;
};

struct VS_OUTPUT
{
	float2 vTexcoord		: TEXCOORD0;
	float2 vTexcoord2		: TEXCOORD1;
	float4 vDiffuse			: TEXCOORD2;
	float3 vViewPosition 	: TEXCOORD3;
	float3 vNormalVS		: TEXCOORD4;
	float3 vWorldPosition	: TEXCOORD5;
	float4 vPosition		: SV_POSITION;
};

/** Transforms a pre-transformed xyzrhw-coordinate into d3d11-space */
float4 TransformXYZRHW(float4 xyzrhw)
{
	// MAGIC (:
	
	// Convert from viewport-coordinates to normalized device coordinates
	float3 ndc;
	ndc.x = ((2 * (xyzrhw.x - V_ViewportPos.x)) / V_ViewportSize.x) - 1;
	ndc.y = 1 - ((2 * (xyzrhw.y - V_ViewportPos.y)) / V_ViewportSize.y);
	ndc.z = xyzrhw.z;
	
	// Convert to clip-space. rhw is actually 1/w ("reciprocal"). So to undo the devide by w, devide by the given 1/w.
	float actualW = 1.0f / xyzrhw.w;
	float3 clipSpace = ndc.xyz * actualW;
	
	return float4(clipSpace, actualW);
		
	// Remove viewport-transformation
	/*xyzrhw.xy -= FF_ViewportPos;
	xyzrhw.xy = xyzrhw.xy * 2.0f - FF_ViewportSize;
	
	// We don't want this in pixels
	xyzrhw.xy /= FF_ViewportSize; 
	
	// D3D11 will turn this upside down later, so counter that here!
	xyzrhw.y = -xyzrhw.y;
	
	// Remove the stupid half-pixel offset from pre D3D10
	xyzrhw.xy -= 0.5f / FF_ViewportSize;*/
	
	// Remove w-component, as it can mess things up when not 1. (Why? Not sure, sorry)
	return float4(xyzrhw.xyz, xyzrhw.w);
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	Output.vPosition = TransformXYZRHW(float4(Input.vPosition, Input.vNormal.x)); // rhw is stored in normal.x
	
	//Output.vPosition = mul( float4(Input.vPosition,1), M_WorldViewProj );
	Output.vTexcoord2 = Input.vTex1;
	Output.vTexcoord = Input.vTex1;
	Output.vDiffuse  = Input.vDiffuse;
	Output.vNormalVS = float3(0,0,0);//mul(Input.vNormal, (float3x3)M_WorldView);
	Output.vViewPosition = float3(0,0,0);//mul(float4(Input.vPosition,1), M_WorldView);
	Output.vWorldPosition = Input.vPosition;
	
	return Output;
}

