#include "Shader_Defines.hlsli"


matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D       g_NoiseTexture;
texture2D       g_RGBTexture;
float2          g_vAddUVPos;


texture2D       g_DissolveTexture;
float           g_fDissolveWeight;
float4          g_vDissolveColor = { 1.0f, 0.5f, 1.0f, 1.f };

bool            g_bCustomColor = false;
float4          g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT; //! ÅºÁ¨Æ®!
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; //! ¿ùµåÀ§Ä¡!
    float4		vProjPos : TEXCOORD2;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
    float4      vNormal :   NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4      vWorldPos : TEXCOORD1;
    float4      vProjPos :  TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* ÇÈ¼¿¼ÎÀÌ´õ : ÇÈ¼¿ÀÇ »ö!!!! À» °áÁ¤ÇÑ´Ù. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

    return Out;
}

/* ÇÈ¼¿¼ÎÀÌ´õ : ÇÈ¼¿ÀÇ »ö!!!! À» °áÁ¤ÇÑ´Ù. */
PS_OUT PS_BREATH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vTexNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + g_vAddUVPos);

    if (0.1f >= vTexNoise.r)
        discard;

    Out.vDiffuse = vMtrlDiffuse * vTexNoise;
    
    if (vMtrlDiffuse.r == 0.0f && vMtrlDiffuse.g == 0.0f && vMtrlDiffuse.b == 0.0f)
        discard;
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    
    
    Out.vDiffuse = vMtrlDiffuse;
    
    //float t = saturate(In.vNormal.y); // Y ÁÂÇ¥¸¦ »ç¿ëÇÏ¿© º¸°£ ºñÀ² °è»ê
    //float3 redColor = float3(1.0f, 0.0f, 0.0f);
    //float3 yellowColor = float3(1.0f, 1.0f, 0.0f);
    //Out.vDiffuse.rgb = g_RGBTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

    return Out;
}

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / 1000.0f;
	
    return Out;
}

PS_OUT PS_MAIN_CUSTOM_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    if (true == g_bCustomColor)
    {
        vMtrlDiffuse.rgb = g_vColor.rgb;
    }
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

    return Out;


}

PS_OUT PS_DISSOLVE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDissolve = g_DissolveTexture.Sample(ClampSampler, In.vTexcoord);

    if (vDissolve.r <= g_fDissolveWeight)
        discard;

    if ((vDissolve.r - g_fDissolveWeight) < 0.1f)
        Out.vDiffuse = g_vDissolveColor;
    else
        Out.vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
   
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    
    if (0 == Out.vDiffuse.a)
        discard;

    return Out;
    
}

PS_OUT PS_DISSOLVE_PLUS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
    float4 vDissolve = g_DissolveTexture.Sample(ClampSampler, In.vTexcoord);

    float sinTime = sin(g_fDissolveWeight);
    
    if (vColor.a == 0.f)
        clip(-1);

    if (vDissolve.r >= sinTime)
        vColor.a = 1;
    else
        vColor.a = 0;

    if (vDissolve.r >= sinTime - 0.05 && vDissolve.r <= sinTime + 0.05)
        vColor = float4(1, 0, 0, 1); // »¡
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // ³ë
    else;

    if (vDissolve.r >= sinTime - 0.025 && vDissolve.r <= sinTime + 0.025)
        vColor = float4(1, 1, 1, 1); // Èò
    else;

    Out.vDiffuse = vColor;
    
    if (0 == Out.vDiffuse.a)
        discard;
   
    return Out;
};

technique11 DefaultTechnique
{
	pass Model // 0
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass ModelAlpha // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ModelAlphaCullNone // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_NoneCull_NoneDSS // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_NoneCull // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_NoneCull_NoneBlack_FireColor // 5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BREATH();
    }

    pass Shadow // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass Model_CustomColor // 7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CUSTOM_COLOR();
    }

    pass DissolveBasic // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
    }

    pass DissolvePlus // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_PLUS();
    }
}
