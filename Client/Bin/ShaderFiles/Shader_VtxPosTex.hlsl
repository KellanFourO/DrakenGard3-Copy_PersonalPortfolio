#include "Shader_Defines.hlsli"
//TODO ���̴������� Vector
//!���̴������� Vector�� float2, float3, float4 �� ����� �� �ִ�. �� �߿����� float4 �ڷ����� vector�� ����.
//! vector.x �� vector.r�� ����.
//! x,y,z,w == r,g,b,a ó�� ����� �� ������ ����ϱ⿡ ���� �ؼ��ϴ� �ǹ̴� �޶��� �� �ִ�.
//! vector.xy = 1.f <-- ���̴������� vector�� �ѹ��� ���� ���� ������ �� �ִ�. x,y�� 1�� ���� �ٲ� ��

//TODO ���̴������� Matrix
//! ���̴������� Matrix�� float4x4�� ����. ���������� vectoró�� ���� ���� ä��� ������ �� ������ �ʴ´�.

//TODO Ŭ���̾�Ʈ�κ��� ���� ������ ������ ���������� Ŭ���̾�Ʈ���� �Ѱܹ��� ����, ��, ��������� ��� ���� ��������
//! ���̴������� ���������� ������̺�(Constant Table)�̶�� �Ҹ����. 
//! �� �״��, ���ȭ �Ǿ ���� ����� �����ϳ� ������ �Ұ����ϴ�. �׷��� Ŭ���̾�Ʈ���� �����ִ� ��.
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D       g_Texture[2];

texture2D       g_DiffuseTexture;
texture2D       g_MaskTexture;
texture2D       g_NoiseTexture;
texture2D       g_DepthTexture;
texture2D       g_DissolveTexture;

float			g_fAtlasPosX;
float			g_fAtlasPosY;
float			g_fAtlasSizeX;
float			g_fAtlasSizeY;

float2          g_UVOffset;
float2          g_UVScale;

float           g_fUVAnimX;
float           g_fUVAnimY;
bool            g_bIsRtoL;

vector          g_vCamDirection;

//HP_UI
float           g_fCrntHPUV;
float           g_fPrevHPUV;

float           g_fTimeDelta;

bool            g_bCustomColor = false;
float4          g_vColor;

float           g_fDissolveWeight;
float4          g_vDissolveColor = { 0.7f, 0.0f, 0.1f, 1.0f };

//TODO ���̴��� �ϴ� ��
//! ������ ��ȯ ( ���庯ȯ, �亯ȯ, ������ȯ ) �� �����Ѵ�. ( ������� ��������� ���ߴٰ� ���� �����̽��� �ִ� ���̾ƴϴ�. �ݵ�� w������ ���� ��ģ �������� ���� �����̽� ��ȯ�� �ƴٰ� �� �� �ִ�. )
//! ������ ���������� �߰�, ���� ���� ������ �����Ѵ�.

struct VS_IN
{
	float3		vPosition : POSITION; //! ��ġ���� ��Ÿ���� �ø�ƽ, ���� ������ �ؾ��ϴ� �ڷ������� �˸�.
	float2		vTexCoord : TEXCOORD0; //! �ؽ�ó�� ��ǥ���� ��Ÿ���� �ø�ƽ, UV ��ǥ��� 0 ~ 1������ ���� ������ ��, Ư�� ��ġ�� ������ �˱�����?
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! �������ֱ����� ��ġ. ������������������ ���� �� w���� �ִ�ä�����ؼ� float4�� �ٲ����, SV_POSITION �ø�ƽ�� �༭ ������ ���� �ڷ����̶� ���� ���̴����� �˸�
	float2		vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; //! HLSL������ ��밡���� ����ü ���θ޸�
	
	//! �Ű����ڷ� ���� In�� ���� �����̴�
	//! In.vPosition * ���� * �� * ������ ������Ѵ�.
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//TODO mul �Լ��� ��İ��� ������ ��� ��ĵ��� ������ �������ش�. 

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); //! Ŭ�󿡼� ������ ������ ��ġ������ ��İ��� �����ϰ� ���� �����ְ� w���̴� 1�� �ʱ�ȭ ������ ��İ� ����,��,���� ���� ��ģ ����� �����ش�.
	Out.vTexCoord = In.vTexCoord; //! �ؽ����� ������ �ʴ´ٴ� ���� �� �� �ִ�.
	
	return Out;
}

//TODO ����
//! ������ �׸��� �� �ᱹ �ε����� �׸��Ŵ�.
//! �簢���� ���÷� ���ڸ�  ���� 4��, �ε��� 6���� ������ �ִ�. ������ { 0,1,2,3 } �ε����� { {0,1,2}, {0,2,3} }�� ������ �ִ�.
//! 0��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! 1��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! 2��° ������ ������ VS_IN���� ����
//! ����� ������ ���
//! ���� ����� ������ ������ 3���� �Ǿ����� �׸��� �۾��� �����Ѵ� ( Why? �ۿ��� Ʈ���̾ޱ۸���Ʈ�� �׸��ٰ� �����ϱ� ( �ﰢ������ �׸��� ) )
//! ���� 3���� ���ؼ� w������ ��ȯ, ����Ʈ ��ȯ, ���� ���� �� ���� ���� �� ä��ٴ� �۾��帧�� �̾���.
//! �������̴��� 6�� ���
//! �ȼм��̴��� ���� ������ * ���� ������ ���� 800 * 600 �� �� = 480,000 �� ȣ��

struct PS_IN
{
	float4		vPosition : SV_POSITION; //! �� PS_IN�ε� ������ �����ٴ� �ø�ƽ����? �����Ͷ����� ������ ���ƴٰ� �ϸ� �翬�� �ȼ�ȭ�� �� �����̴� ������ ������.
	float2		vTexCoord :	TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! �ٷ�9������ COLOR��� �߾����� 11���� �ٲ����. �ξ� �������� �������ٰ� �� �� �ִ�. ��? ���° ����Ÿ�ٿ� �׸����� ��� �������̴ϱ�
};

//TODO �ȼ� ���̴� : �ȼ��� ��!!!!!�� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    vector vSourColor = g_Texture[0].Sample(LinearSampler, In.vTexCoord);
    vector vDestColor = g_Texture[1].Sample(LinearSampler, In.vTexCoord);

	Out.vColor = vSourColor + vDestColor; //! ���� �ؽ�ó�ε� �������� �׳� ���������� ä��.
	
	return Out;
}



PS_OUT PS_MAIN_ATLAS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
   // Atlas UV PickUP
    if (In.vTexCoord.x < g_fAtlasPosX || In.vTexCoord.y < g_fAtlasPosY)
        discard;
    if (g_fAtlasSizeX < In.vTexCoord.x || g_fAtlasSizeY < In.vTexCoord.y)
        discard;

	// Set Color
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    //Out.vColor += g_vecColorOverlay / 255.f;

	// Alpha Test
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    //if (g_bGrayScale)
    //{
    //    Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
    //}
    return Out;
}

PS_OUT PS_MAIN_ATLAS_GRID(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Atlas UV PickUP
    if (In.vTexCoord.x < g_fAtlasPosX || In.vTexCoord.y < g_fAtlasPosY)
        discard;
    if (g_fAtlasSizeX < In.vTexCoord.x || g_fAtlasSizeY < In.vTexCoord.y)
        discard;

	// Set Color
    Out.vColor = g_Texture[0].Sample(LinearSampler, In.vTexCoord);
    //Out.vColor += g_vecColorOverlay / 255.f;
    Out.vColor.a = 0.1f;

    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_ATLAS_ANIMATION(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;



    float2 clippedTexCoord = In.vTexcoord * g_UVScale + g_UVOffset;

    // Set Color
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, clippedTexCoord);

    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;
    // Alpha Test
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    return Out;
}

PS_OUT PS_MAIN_ATLAS_ANIMATION_TIMEALPHA(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;



    float2 clippedTexCoord = In.vTexcoord * g_UVScale + g_UVOffset;

    // Set Color
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, clippedTexCoord);

    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;
    
    
    Out.vColor.a *= 0.4f;
    // Alpha Test
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    
    
    return Out;
}

PS_OUT PS_MAIN_ATLAS_ANIMATION_USEMASK_USENOISE(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;



    float2 clippedTexCoord = In.vTexcoord * g_UVScale + g_UVOffset;

    // Set Color
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, clippedTexCoord);

    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;
    
    
    Out.vColor.a *= 0.4f;
    // Alpha Test
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    
    
    return Out;
}



struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    matrix WorldMatrix = g_WorldMatrix;
    
    float3 vLook = normalize((g_vCamDirection * -1.f).xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3 vUp = normalize(cross(vLook, vRight));

    WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
    WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
    WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);
    
	/* In.vPosition * ���� * �� * ���� */
    matrix matWV, matWVP;

    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexCoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT_EFFECT VS_MAIN_EFFECT_NONBILLBOARD(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;
    
	/* In.vPosition * ���� * �� * ���� */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexCoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}



/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if(true == g_bCustomColor)
        Out.vColor.rgb = g_vColor.rgb;
    
    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;
    
    if (Out.vColor.a < 0.3f)
    {
        discard;
    }
    
    return Out;
}


/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_EFFECT_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    //Out.vColor.rgb = float3(1.0, 0.8, 0.8);
    
    //Out.vColor = float4(1.0, 0.8, 0.8, 1.f);
    Out.vColor = g_vColor;
    float4 vMaskDesc = g_MaskTexture.Sample(LinearSampler, (In.vTexCoord * -1));
    float4 vNoiseDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexCoord * -1);
  
    
    //Out.vColor.a *= vMaskDesc.x;
    Out.vColor.a = max(vMaskDesc.x, 0.f);
    
    //if(Out.vColor.a < 0.3)
    //    discard;
    
    //Out.vColor.rgb *= vNoiseDesc.rgb * 1.5f;
   
    return Out;
}

/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_UI_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
   
    float fHp = g_fPrevHPUV;
    
    if(In.vTexCoord.x > fHp)
    {
        discard;
    }
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_UI_HPFrame(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    
    if (Out.vColor.a < 0.2f)
        discard;
    
        return Out;
}


PS_OUT PS_MAIN_EFFECT_USEMASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (true == g_bCustomColor)
        Out.vColor.rgb = g_vColor.rgb;
    
    Out.vColor.a *= g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
    
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_MAIN_EFFECT_USEMASK_USENOISE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

    if (true == g_bCustomColor)
        Out.vColor.rgb = g_vColor.rgb;
    
    //Out.vColor.a *= g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
    //Out.vColor.rgb *= g_NoiseTexture.Sample(LinearSampler, In.vTexCoord);
    
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_DISSOLVE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    
    
    vector vDissolve = g_DissolveTexture.Sample(ClampSampler, In.vTexCoord);
    vector vDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexCoord);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    if (vDissolve.r <= g_fDissolveWeight)
        discard;

    if ((vDissolve.r - g_fDissolveWeight) < 0.1f)
        Out.vColor = g_vDissolveColor;
    else
    {
        Out.vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexCoord);
        
            if (true == g_bCustomColor)
            Out.vColor.rgb = g_vColor.rgb;
    }
   
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    return Out;
    
}

PS_OUT PS_DISSOLVE_PLUS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexCoord);
    float4 vDissolve = g_DissolveTexture.Sample(ClampSampler, In.vTexCoord);

    float sinTime = sin(g_fDissolveWeight);
    
    if (vColor.a == 0.f)
        clip(-1);

    if (vDissolve.r >= sinTime)
        vColor.a = 1;
    else
        vColor.a = 0;

    if (vDissolve.r >= sinTime - 0.05 && vDissolve.r <= sinTime + 0.05)
        vColor = float4(1, 0, 0, 1); // ��
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // ��
    else;

    if (vDissolve.r >= sinTime - 0.025 && vDissolve.r <= sinTime + 0.025)
        vColor = float4(1, 1, 1, 1); // ��
    else;

    Out.vColor = vColor;
    
    if (0 == Out.vColor.a)
        discard;
   
    return Out;
};


technique11 DefaultTechnique //! �ٷ�9 ���ķ� ��ũ��ũ�ڿ� ������ �ٿ������. �츰 �ٷ�11�̴� 11�� �ٿ���
{
	pass UI // 0�� �н�
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

	/* ���� �ٸ� ���¿� ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
	pass Particle //1�� �н�
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

/* ���� �ٸ� ���¿� ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
    pass Effect // 2�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass Atlas_Default //3�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS();
    }

    pass Atlas_Grid // 4�� �н�
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_GRID();
    }


    pass Atlas_Animation //5�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION();
    }

    pass Effect_Trail //6�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_TRAIL();
    }


    pass UI_HP //7�� �н�
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_HP();
    }

    pass UI_HPFrame //8�� �н�
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_HPFrame();
    }

    pass Atlas_Animation_TimeAlpha //9�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION_TIMEALPHA();
    }

    pass Atlas_Animation_UseMask_UseNoise //10�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION_USEMASK_USENOISE();
    }

    pass Effect_UseMask // 11�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_USEMASK();
    }

    pass Effect_UseMask_UseNoise // 12�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT_NONBILLBOARD();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_USEMASK_USENOISE();
    }

    pass DissolveBasic // 13�� �н�
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT_NONBILLBOARD();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
    }

    pass DissolvePlus // 14�� �н�
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT_NONBILLBOARD();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_PLUS();
    }
};