
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f); //! ������ Ŭ�󿡼� ��������Ѵ�. ���Ƿ� ���� ���� ���� ����
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f); //! ���Ƿ� ���� ���� ��

//TODO �����Ʈ �÷��� �������� �����̴�.
//! �����Ʈ ������ ���� ���� �������ϴ� �κ��� �ʹ� ��Ӱ� �׷����� ������ ���Ǽ��� �������Ƿ� �����ϱ����� ������ �����Ѵ�
//! ���� ���̴��� ����ϴ� ��� ��ü�� �Ʒ����� ������ �����Ʈ �÷����� ������ �޴´ٰ� ����ȴ�.
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f); //! �ݻ翥���Ʈ

//TODO �ؽ�ó�� �����꿡 �ʿ��� �ȼ��� ��ǻ������� ���޹ޱ����� �����̴�. �׷��� �������� �������ߴ�
//!texture2D		g_Texture;
texture2D		g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR; //! ���Ͼ�� �־ �ӷ����� ���� �ؽ�ó����( dx11������ dds�� ����)�� ���Դٸ� ���Ͼ �൵ ������� �ʴ´�.
	AddressU = wrap;
	AddressV = wrap;
};

//TODO ���̴��� �ϴ� ��
//! ������ ��ȯ ( ���庯ȯ, �亯ȯ, ������ȯ ) �� �����Ѵ�. ( ������� ��������� ���ߴٰ� ���� �����̽��� �ִ� ���̾ƴϴ�. �ݵ�� w������ ���� ��ģ �������� ���� �����̽� ��ȯ�� �ƴٰ� �� �� �ִ�. )
//! ������ ���������� �߰�, ���� ���� ������ �����Ѵ�.

struct VS_IN
{
	float3		vPosition : POSITION; //! ��ġ���� ��Ÿ���� �ø�ƽ, ���� ������ �ؾ��ϴ� �ڷ������� �˸�.
	float3		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0; //! �ؽ�ó�� ��ǥ���� ��Ÿ���� �ø�ƽ, UV ��ǥ��� 0 ~ 1������ ���� ������ ��, Ư�� ��ġ�� ������ �˱�����?
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! �������ֱ����� ��ġ. ������������������ ���� �� w���� �ִ�ä�����ؼ� float4�� �ٲ����, SV_POSITION �ø�ƽ�� �༭ ������ ���� �ڷ����̶� ���� ���̴����� �˸�
	float3		vNormal	  : NORMAL;
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
	//! �븻�� ���⺤�ʹϱ� w���� �ʿ��� 0. ���� ���⺤�ʹ� ������ǥ�迡 �ִ�. �Ŀ� �븻�� ���� ���⺤�͸� ������ ������ ������ �ִµ� ���� ��ǥ��� �����ֱ����� In.vNormal�� ������ǥ��� �ø���.
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
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
	float3		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! �ٷ�9������ COLOR��� �߾����� 11���� �ٲ����. �ξ� �������� �������ٰ� �� �� �ִ�. ��? ���° ����Ÿ�ٿ� �׸����� ��� �������̴ϱ�
};

//TODO �ȼ� ���̴� : �ȼ��� ��!!!!!�� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	//!�̰��� �ٽ� �����ؾ��Ѵ�. �������� ���� �÷����� �ƴ� �ؽ�ó�κ��� �о�� �ݻ��� Diffuse ������ �ǹ��Ѵ�.
	//Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	//! �ٽ� ���ǵ� ����
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

	return Out;
}

technique11 DefaultTechnique //! �ٷ�9 ���ķ� ��ũ��ũ�ڿ� ������ �ٿ������. �츰 �ٷ�11�̴� 11�� �ٿ���
{
	pass UI
	{
		//! ����������Ʈ�� �ü��� ����.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	/* ���� �ٸ� ���¿� ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
	pass Particle
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};