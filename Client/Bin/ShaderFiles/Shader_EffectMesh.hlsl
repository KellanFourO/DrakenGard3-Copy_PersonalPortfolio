//#include "Client_Shader_Defines.hpp"
#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamDirection;

texture2D g_DiffuseTexture;
texture2D g_NoiseTexture;
texture2D g_MaskTexture;

texture2D g_DepthTexture;
float4      g_vShaderFlag = { 0.f, 0.f, 0.f, 0.f };
// TODO :  bDynamicNoiseOption temporary for test
bool g_bDynamicNoiseOption;





/**
* Wrap Weight for Textures
*  x : Diff, y : Noise, z : Mask, w : None
*/
vector g_vWrapWeight;


bool g_bDiffuseWrap;
bool g_bNoiseWrap;
bool g_bMaskWrap;

float2 g_vUVDiff;
float2 g_vUVNoise;
float2 g_vUVMask;

float g_fDiscardRatio;

float4 g_vColor;

bool g_bBillboard;

// Vertex Shaders //

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};



VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix WorldMatrix = g_WorldMatrix;

    if (g_bBillboard)
    {
        float3 vLook = normalize((g_vCamDirection * -1.f).xyz);
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        float3 vUp = normalize(cross(vLook, vRight));

        WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
        WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
        WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);
    }

    matrix matWV, matWVP;

    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}

VS_OUT_SOFT VS_MAIN_SOFT(VS_IN In)
{
    VS_OUT_SOFT Out = (VS_OUT_SOFT) 0;

    matrix WorldMatrix = g_WorldMatrix;

    if (g_bBillboard)
    {
        float3 vLook = normalize((g_vCamDirection * -1.f).xyz);
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        float3 vUp = normalize(cross(vLook, vRight));

        WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
        WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
        WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);
    }

    matrix matWV, matWVP;

    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;

    return Out;
}


//  Pixel  Shaders  //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_IN_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vShaderFlag : SV_TARGET3;
};


PS_OUT PS_MAIN_SOFT(PS_IN_SOFT In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_bDiffuseWrap)
        Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.x + g_vUVDiff);
    else
        Out.vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.x + g_vUVDiff);

    vector vNoise = (vector) 0;
    if (g_bNoiseWrap)
        vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.y + g_vUVNoise);
    else
        vNoise = g_NoiseTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.y + g_vUVNoise);

    vector vMask = (vector) 0;
    if (g_bMaskWrap)
        vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.z + g_vUVMask);
    else
        vMask = g_MaskTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.z + g_vUVMask);

	// (0, +1) => (-1, +1)
    if (g_bDynamicNoiseOption)
        vNoise.rgb = vNoise.rgb * 2 - 1;

    Out.vColor *= g_vColor;
    Out.vColor.rgb *= vNoise.rgb;
    Out.vColor.a *= vMask.r;

    if (g_fDiscardRatio > Out.vColor.a)
        discard;

    float2 vTexUV;

    vTexUV.x = In.vProjPos.x / In.vProjPos.w;
    vTexUV.y = In.vProjPos.y / In.vProjPos.w;

    vTexUV.x = vTexUV.x * 0.5f + 0.5f;
    vTexUV.y = vTexUV.y * -0.5f + 0.5f;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);
    float fViewZ = vDepthDesc.y * 300.f;
    Out.vColor.a = Out.vColor.a * saturate(fViewZ - In.vProjPos.w);

    //if (g_bBloom)
    //    Out.vExtractBloom = Out.vColor;
    //
    //if (g_bGlow)
    //    Out.vExtractGlow = g_vGlowColor;

    Out.vShaderFlag = g_vShaderFlag;

    return Out;
}

PS_OUT PS_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    //if (g_bDiffuseWrap)
    //    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.x + g_vUVDiff);
    //else
    //    Out.vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.x + g_vUVDiff);
    //
    //vector vNoise = (vector) 0;
    //if (g_bNoiseWrap)
    //    vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.y + g_vUVNoise);
    //else
    //    vNoise = g_NoiseTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.y + g_vUVNoise);
    //
    //vector vMask = (vector) 0;
    //if (g_bMaskWrap)
    //    vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV * g_vWrapWeight.z + g_vUVMask);
    //else
    //    vMask = g_MaskTexture.Sample(ClampSampler, In.vTexUV * g_vWrapWeight.z + g_vUVMask);

	// (0, +1) => (-1, +1)
    //if (g_bDynamicNoiseOption)
    //    vNoise.rgb = vNoise.rgb * 2 - 1;

    Out.vColor *= g_vColor;
    //Out.vColor.rgb *= vNoise.rgb;
    //Out.vColor.a *= vMask.r;

    if (g_fDiscardRatio > Out.vColor.a)
        discard;

    //if (g_bBloom)
    //    Out.vExtractBloom = Out.vColor;
    //
    //if (g_bGlow)
    //    Out.vExtractGlow = g_vGlowColor;

    Out.vShaderFlag = g_vShaderFlag;

    return Out;
}


float IsIn_Range(float fMin, float fMax, float fValue)
{
    return (fMin <= fValue) && (fMax >= fValue);
}


// Shader Passes //
technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEFAULT();
    }

    pass SoftEffect // 1
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_SOFT();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT();
    }
}
// Shader Passes //