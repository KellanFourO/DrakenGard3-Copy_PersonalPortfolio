#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE		g_Texture;


struct VS_IN
{
	float3		vPosition : POSITION; 
	float3		vTexCoord : TEXCOORD0; // 큐브텍스처는 X,Y,Z 를 모두 사용한다. 방향벡터를 이용해서 TexCoord 맵핑을 해주니까
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! 리턴해주기위한 위치. 렌더링파이프라인을 수행 후 w값이 있는채여야해서 float4로 바뀌었고, SV_POSITION 시맨틱을 줘서 연산이 끝난 자료형이란 것을 셰이더에게 알림
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
	float4		vPosition : SV_POSITION; //! 왜 PS_IN인데 연산이 끝낫다는 시맨틱으로? 래스터라이즈 과정을 거쳤다고 하면 당연히 픽셀화가 된 이후이니 연산이 끝났다.
	float3		vTexCoord :	TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! 다렉9에서는 COLOR라고 했었으나 11에서 바뀌었다. 훨씬 가독성이 좋아졌다고 할 수 있다. 왜? 몇번째 렌더타겟에 그릴꺼냐 라는 직관적이니까
};

//TODO 픽셀 셰이더 : 픽셀의 색!!!!!을 결정한다.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord); //! 아직 텍스처로드 못했으니 그냥 빨간색으로 채움.
	
	return Out;
}

technique11 DefaultTechnique //! 다렉9 이후로 테크니크뒤에 버전을 붙여줘야함. 우린 다렉11이니 11로 붙여줌
{
	pass SkyBox
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		//! 렌더스테이트가 올수도 있음.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
};