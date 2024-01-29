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
texture2D       g_DepthTexture;

float			g_fAtlasPosX;
float			g_fAtlasPosY;
float			g_fAtlasSizeX;
float			g_fAtlasSizeY;





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
    //if (In.vTexCoord.x < g_fAtlasPosX || In.vTexCoord.y < g_fAtlasPosY)
    //    discard;
    //if (g_fAtlasSizeX < In.vTexCoord.x || g_fAtlasSizeY < In.vTexCoord.y)
    //    discard;

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

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
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

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    float2 vDepthTexcoord;
    vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
    Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;

    return Out;
}

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
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
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

};