#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

//TODO �� 256�̾�? #����Ʈ����256
//! ������ �ý��۸޸𸮸� ����Ҷ����� ����޸𸮸� �̿��ؼ� ������� Ŀ���� �����߾���.
//! ������ ���̴��� �׷���ī�带 ����ϴ� GPU������ �ϹǷ�, ����޸𸮸� ������� ���ϴ� ������, VRAM�̶�� �޸𸮿����� ����Ѵ�.
//! ��, VRAM�� �ʰ��ؼ� ����Ѵٸ� ���� ������ �߱��ϹǷ� ���� ���� �̻��� ������� ���� �ʴ´ٶ�� ������ �Ѱ��̴�. 2�ǽ¼��� ����ؾߵǴ� ���� �ƴϰ� 300, 500���� ���� �� ���ִ�. ���Ƿ� ���ߴٴ� �̾߱��.
//! �Ʒ��� ������� �� ���� ��� �Ž��� ������� �ƴ�, Ư�� �Ž����� ������ �ִ� ������� ������ ���Ѵ�.
//! ����, Ư�� �Ž����� ������ �ִ� ������� ������ �ʹ� ���ٸ�, VTF(���ؽ� �ؽ�ó ��ġ)��� ����� ����ؾ��Ѵ�.
//! VTF ����� �ؽ�ó���ٰ� ����� �޾Ƽ� �� �ؽ�ó�� ���̴����� �ޝҾƵ��̸� �� ������ ���� �� �ִ� �����̴�.

matrix			g_BoneMatrices[256]; //! �𵨿��� ������ ���̾ƴϴ�. ������ �Ž����� ������ ���̰� ���� ���ؼ� �޾ƿ��� ���� ���̴�.

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
	float3		vTangent : TANGENT; //! ź��Ʈ!

	uint4		vBlendIndices : BLENDINDEX; //! ������ �ִ� ������ �ε����� ��Ƴ���
	float4		vBlendWeights : BLENDWEIGHT; //! ������ �ִ� ���鸶���� ����ġ�� ��Ƴ���
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; //! ������ġ!
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	//! �ִԸ��� �Ž��� ���� ���� ���� ������, ������ ������쿡 ������ 1�� ���� �������� ����� �׵����� �������� �´�. ������ ����ġ�� ����
	//! ��ó�� x,y,z�� �� ���ؼ� ���� ������� 1�̴�. 0���� ä������ ���� ��쿡�� 1�� ���� ��.
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z); 
	

	//! ������ �� �ϳ��� �ƴ� ������ �ִ� ��(�ִ� 4��)�� ������ ȥ���ؼ� ����� ����� ���� �Ѵ�.
	//! ȥ���Ҷ� ���ϱ� �������� �ϸ� ����ϴ�.
	//! �� ȥ���� �ϴ� �� ��Ű���̶�� �ϱ⵵�ϰ�, ��Ű���� ���̴����� �۾��߰�. �ϵ���� ��Ű���۾��� �ߴٰ� ���� �� �ִ� ��.
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	//! ������ ������ ������� �����شٰ��ߴ�.
	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//! ������� ������ ����(vPosition) �������� �׸���.
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

/* �ȼ����̴� : �ȼ��� ��!!!! �� �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if(vMtrlDiffuse.a < 0.3f)
		discard;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�. */
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
