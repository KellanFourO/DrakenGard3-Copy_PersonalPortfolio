#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE		g_Texture;


struct VS_IN
{
	float3		vPosition : POSITION; 
	float3		vTexCoord : TEXCOORD0; // ť���ؽ�ó�� X,Y,Z �� ��� ����Ѵ�. ���⺤�͸� �̿��ؼ� TexCoord ������ ���ִϱ�
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! �������ֱ����� ��ġ. ������������������ ���� �� w���� �ִ�ä�����ؼ� float4�� �ٲ����, SV_POSITION �ø�ƽ�� �༭ ������ ���� �ڷ����̶� ���� ���̴����� �˸�
	float3		vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; 
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); 
	Out.vTexCoord = In.vTexCoord;
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION; //! �� PS_IN�ε� ������ �����ٴ� �ø�ƽ����? �����Ͷ����� ������ ���ƴٰ� �ϸ� �翬�� �ȼ�ȭ�� �� �����̴� ������ ������.
	float3		vTexCoord :	TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! �ٷ�9������ COLOR��� �߾����� 11���� �ٲ����. �ξ� �������� �������ٰ� �� �� �ִ�. ��? ���° ����Ÿ�ٿ� �׸����� ��� �������̴ϱ�
};

//TODO �ȼ� ���̴� : �ȼ��� ��!!!!!�� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord); //! ���� �ؽ�ó�ε� �������� �׳� ���������� ä��.
	
	return Out;
}

technique11 DefaultTechnique //! �ٷ�9 ���ķ� ��ũ��ũ�ڿ� ������ �ٿ������. �츰 �ٷ�11�̴� 11�� �ٿ���
{
	pass SkyBox
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		//! ����������Ʈ�� �ü��� ����.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
};