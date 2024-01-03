#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

//TODO 왜 256이야? #본매트리스256
//! 기존에 시스템메모리를 사용할때에는 가상메모리를 이용해서 어느정도 커버가 가능했었다.
//! 하지만 셰이더는 그래픽카드를 사용하는 GPU연산을 하므로, 가상메모리를 사용하지 못하는 구조고, VRAM이라는 메모리영역을 사용한다.
//! 즉, VRAM을 초과해서 사용한다면 많은 문제를 야기하므로 일정 범위 이상의 뼈행렬을 받지 않는다라는 행위를 한것이다. 2의승수를 사용해야되는 것은 아니고 300, 500같은 값도 줄 수있다. 임의로 정했다는 이야기다.
//! 아래의 뼈행렬은 한 모델의 모든 매쉬의 뼈행렬이 아닌, 특정 매쉬에게 영향을 주는 뼈행렬의 개수를 말한다.
//! 만약, 특정 매쉬에게 영향을 주는 뼈행렬의 개수가 너무 많다면, VTF(버텍스 텍스처 패치)라는 기법을 사용해야한다.
//! VTF 기법은 텍스처에다가 행렬을 받아서 그 텍스처를 셰이더에서 받앋아들이면 더 가볍게 받을 수 있는 개념이다.

matrix			g_BoneMatrices[256]; //! 모델에서 던지는 것이아니다. 실제로 매쉬에서 던지는 것이고 모델을 통해서 받아오는 것일 뿐이다.

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vCamPosition;


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
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	//! 애님모델의 매쉬중 뼈가 따로 없는 스워드, 쉐도우 같은경우에 강제로 1로 만들어서 보내지만 행렬은 항등으로 곱해져서 온다. 강제로 가중치를 주자
	//! 어처피 x,y,z를 다 더해서 나온 결과값은 1이다. 0으로 채워져서 오는 경우에는 1로 만든 것.
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z); 
	

	//! 정점은 뼈 하나가 아닌 영향을 주는 뼈(최대 4개)의 정보를 혼합해서 행렬을 만들어 내야 한다.
	//! 혼합할때 더하기 연산으로 하면 깔끔하다.
	//! 이 혼합을 하는 것 스키닝이라고 하기도하고, 스키닝을 셰이더에서 작업했고. 하드웨어 스키닝작업을 했다고 말할 수 있는 것.
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	//! 기존의 정점에 뼈행렬을 곱해준다고했다.
	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//! 뼈행렬을 곱해준 정점(vPosition) 기준으로 그리자.
	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if(vMtrlDiffuse.a < 0.3f)
		discard;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다. */
	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    Out.vColor.a = 0.5f;

	return Out;
}


technique11 DefaultTechnique
{
	pass Model
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
	
    pass ModelHide
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
}
