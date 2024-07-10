#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_SpecularTexture;
texture2D       g_DissolveTexture;


matrix			g_BoneMatrices[256]; 
float           g_fDissolveWeight;
float4          g_vDissolveColor = { 1.0f, 0.5f, 1.0f, 1.f };

//플레이어 전용
texture2D       g_BloodTexture;
int             g_iBloodCount = 0;
bool            g_bCustomColor = false;
float4          g_vColor;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT; //! 탄젠트!

	uint4		vBlendIndices : BLENDINDEX; //! 영향을 주는 뼈들의 인덱스를 모아놓자
	float4		vBlendWeights : BLENDWEIGHT; //! 영향을 주는 뼈들마다의 가중치를 모아놓자
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; //! 월드위치!
    float4		vProjPos : TEXCOORD2;
    float4		vTangent : TANGENT;
    float4		vBinormal : BINORMAL;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z); 
	
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//! 뼈행렬을 곱해준 정점(vPosition) 기준으로 그리자.
	Out.vPosition = mul(vPosition, matWVP);
	//Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
    float4		vProjPos : TEXCOORD2;
    float4		vTangent : TANGENT;
    float4		vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (vMtrlDiffuse.a < 0.3f)
        discard;


    Out.vDiffuse = vMtrlDiffuse;
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
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

    Out.vLightDepth = In.vProjPos.w / 600.0f;
	
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
        vColor = float4(1, 0, 0, 1); // 빨
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // 노
    else;

    if (vDissolve.r >= sinTime - 0.025 && vDissolve.r <= sinTime + 0.025)
        vColor = float4(1, 1, 1, 1); // 흰
    else;

    Out.vDiffuse = vColor;
    
    if (0 == Out.vDiffuse.a)
        discard;
   
    return Out;
};


/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN_PLAYER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    if(true == g_bCustomColor)
        vMtrlDiffuse.rgb = g_vColor.rgb;
    
    //vector vBloodDesc = g_BloodTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    //if (g_iBloodCount != 0)
    //{
    //    float fBloodFactor = g_iBloodCount / 5.0f;
        
    //    vMtrlDiffuse.rgb = lerp(vMtrlDiffuse.rgb, vBloodDesc.rgb, float3(fBloodFactor, fBloodFactor, fBloodFactor));
    //}
    
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (vMtrlDiffuse.a < 0.3f)
        discard;


    Out.vDiffuse = vMtrlDiffuse;
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

    return Out;
}

technique11 DefaultTechnique
{
	pass Model //0
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
	
    pass ModelHide // 1
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

    pass Shadow // 2
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

    pass DissolveBasic // 3
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

    pass DissolvePlus // 4
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

    pass Model_Player //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PLAYER();
    }
}
