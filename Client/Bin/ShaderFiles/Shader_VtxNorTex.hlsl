
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightPos = vector(50.f, 10.f, 50.f, 1.f); //! ������ ��ġ ����
float			g_fLightRange = 30.f; //! ������ ����

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f); //! ������ Ŭ�󿡼� ��������Ѵ�. ���Ƿ� ���� ���� ���� ����
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f); //! ���Ƿ� ���� ���� ��

//TODO �����Ʈ �÷��� �������� �����̴�.
//! �����Ʈ ������ ���� ���� �������ϴ� �κ��� �ʹ� ��Ӱ� �׷����� ������ ���Ǽ��� �������Ƿ� �����ϱ����� ������ �����Ѵ�
//! ���� ���̴��� ����ϴ� ��� ��ü�� �Ʒ����� ������ �����Ʈ �÷����� ������ �޴´ٰ� ����ȴ�.
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);

//TODO ����ŧ���� ���ݻ縦 ǥ���ϴ� �����̴�. 
//! ���ݻ�� ���� �°� ����� ƨ��ٴ� ���� �ǹ��Ѵ�. 
//! �� ƨ�� ���� �� �ü��� ���;��� Ȯ���� �� �� �ִ� ����. ==  ���ݻ翡 ���� ����, ex) ���̶���Ʈ
//! ���� ������ ���� ��ü���� �����κ��� ǥ���ϰ��� �� ���� �ַ� ���ݻ�(����ŧ��)�� �̿��Ѵ�.
//! �翬�� ������ ����ŧ������(�Ϲ������� ���) ��ü���� ����ŧ������(��������)�� �ִ�.
//! ���� �޾����� ��� ǥ���� ���ΰ��� ���� ���׸��󿡵� ����ŧ�� ���� �����ؾ��Ѵٴ� ���̴�.
//! ������� ���� ���߸� �Ͼ�� ����ŧ���� ���� �ϴϱ�
//! ��ǻ��, �����Ʈ ������� ������� ���̶���Ʈ�� ������� �׸��� �����Ӱ� ������ �� �ֵ��� ������� �����̴�.
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f); //! �ݻ翥���Ʈ
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

//TODO �ؽ�ó�� �����꿡 �ʿ��� �ȼ��� ��ǻ������� ���޹ޱ����� �����̴�. �׷��� �������� �������ߴ�
//!texture2D		g_Texture;
//! �Ʒ��������� �迭�� �ؽ�ó�� �޴� ������ ����ũ �ؽ�ó�� �̿��ؼ� ������ �����ؽ�ó�� ��� �׸����̱� ������.
texture2D		g_DiffuseTexture[2];
texture2D		g_MaskTexture;
texture2D		g_BrushTexture;

vector			g_vCamPosition; //! �ݻ纤�͸� ��������� �ʿ��� �ü����͸� ���Ҷ� �ʿ��� ī�޶��� ������ġ
vector			g_vBrushPos = vector(50.f, 0.f, 20.f, 1.f);
float			g_fBrushRange = 10.0f;


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
	float4		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; //! �ü����͸� ���Ϸ��� ī�޶��� ������ġ���� �ȼ��� ������ġ�� ������ϴµ� ���� �����̴ϱ� �ȼ��� ������ġ�� �÷���� �Ѵ�.
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
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); //! ��ġ���ʹϱ� w���� 1�� ä���� ���� Ȯ���� �� �ִ�
	
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
	float4		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
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

	//! ����ũ�ؽ�ó�� ������ �����ؽ�ó�� ��� �׸� ���̱⿡ �ٽ� ���ǵǾ� �ȴ�.
	//vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexCoord); //! ����ũ�� Ÿ�ϸ���Ű�� �������̱⿡ uv��ǥ�� 100�� �������� �ʴ´�.

	//TODO Brush�� ������ �ٷ� ���� �ʴ� ����
	//! ���� ������ ��ǻ�� ���� ��������ϴµ�, ���� ������ ��ġ�� ���� ������ ������� �׷��ֱ� �ٶ���.
	//! ������ ���� ������ �� �ٷ� �����ָ� ���� ������ ��ġ�� ����� �ȼ����� �ƴ� ��� �ȼ����� �귯�� �ؽ�ó�κ��� 
	//! ���� ������ ��Ȳ�̴�.
	//! �׷��� if������ �ȼ��� ������ġ�� �귯���� ��ġ,������ �̿��ؼ� �������� �ȼ��� �������
	vector		vBrush = vector(0.f, 0.f, 0.f, 0.f);
	
	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		//! uv��ǥ�� ���� ��� ������ : ������ �ȼ��� ��ü uv��ǥ�� �����ִ� 
		//! �츮�� ���� �����ȿ��� �귯�� ��ü�� �׸�������Ŵ� ���� ��ƾ��Ѵ�
		float2 vUV;
		
		//! �������� ���Դٴ� ������ ����� �ȼ��� ������ġ�� x�� 356�̶�� �غ���
		//! �귯���� ��ġ.x�� 400, �귯���� ������ 50�̶�� �غ���
		//! 356 - 350 = 6 / 100 = 0.06
		//! �ȼ��� ������ġ.x�� 450�̶�� �غ���
		//! 450 - 350 = 100 / 100 = 1
		//! �� 0 ~ 1 ������ ��ǥ�� ������ �����
		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(DefaultSampler, vUV);
	}

	//!����ũ �ؽ�ó�� �о���� ���� �Ͼ��, �������̳Ŀ� ���� ��ؽ�ó�� ������ �׸������� �����ϱ����� ������̴�
	//! ���� ����ũ�� ������ vector(1.f, 1.f, 1.f, 1.f) �̾��ٸ� 1�� 0�� ���ϴ� ������� ����ũ�� 0�̴�. 1���� 0�� ���� SourDiffuse �״���� �����̳�����
	//! �ݴ��ǰ�쿡�� 1 + 0 * SourDiffuse�� �Ǵ� DestDiffuse ���� ������ ���̴�.

	//TODO ���� �귯�� �������Ǿ��� ���Դٸ� vBrush�� �ؽ�ó�κ��� ������ �����ͼ� �ؽ�ó������ �����ذŰ� �ƴ϶�� 0�ʱ�ȭ�� vBrush���� ���� �״�� ����
	vector		vMtrlDiffuse = vMask * vDestDiffuse + (1.f - vMask) * vSourDiffuse + vBrush;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
	
	//! ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�.
	vector		vLook = In.vWorldPos - g_vCamPosition; //! �ü� ����
	
	//TODO �ݻ纤�͸� �����ִ� �Լ��� �۵�����
	//! �ȼ��� ���������� ���̴� ��� 1�� �����س�����.
	//! ���� �ȼ��� �鿡 ������ ���� ���⺤�͸� �ݴ�� �����´�. �� �ݴ�� ������ ���͸� �����⺤�Ͷ�� ǥ���ϰڴ�.
	//! �����⺤�Ϳ� �������͸� �����ϸ� ���翵�������� ���̰� ���´�.
	//! 1�� ���̸� ���� �������Ϳ��� ���翵���̸� �����ָ� ���������� ���⺤�Ϳ� ���� ���̴� ���翵������ ���Ͱ� ��������µ� �� ���͸� ������Ͷ� �ϰڴ�.
	//! ��� ���͸� �����Ͱ��ƴ� ���� ���� ���� ���⺤�Ϳ��� �����ָ� �����̵����Ͱ� ������, �ѹ� �� �����ָ� �ݻ纤�Ͱ� ��������.
	//! �� ������ �Ʒ� reflect �Լ����� ���ְ��ִ� ��.
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	//TODO �� �ݻ纤�Ϳ� �ü����͸� �̿��� ���������� ���� ������� ����ŧ�� ����
	//! pow(�������Լ�)�� ����� ������ ����ŧ�� ���� �׷����� ���ġ�� �ʹ� ���Գ����⶧���� �ŵ����������༭ �ݻ�Ǵ� ���� ������ ����
	float	fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
				+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}

//TODO �������� ���� �Լ�
//! �츮 ���� ���̵����δ� �������� ǥ���ϱ� ��ٷӴ�. 
//! ������ �ϳ������� �������� �������� �ø� �ü��� �����ӵ���� �Ͼ�� ������ �켱 �ϳ��� �ؼ� �׽�Ʈ�ϰ�
//! ���߿� ���߱����� ���۵���̴� ����� ���� ����Ѵ�.
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//! �������� ���� ������ ����ÿ� �ƿ� ���� �������ƾ��Ѵ�. �����Ʈ ���� ���������̴�.

	//! �������� ���� ������ ���� ��������Ѵ�.
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vLightDir = In.vWorldPos - g_vLightPos; //! �ȼ��� ������ġ�� �ٶ󺸴� ���� ����

	//TODO ���Ⱚ (Attenuation) �����ִ� ��
	//! ���� ������ ���Ⱚ�� 1 , ���� ���� Ŀ���� ���Ⱚ�� 0
	//! ���Ⱚ�� �� ���ϴµ�? ������ �Ÿ������� ��Ӱų� ȯ�� ǥ���� ���� �����ִ� ��.
	//! �������� �������� �������� �ȼ��� �ٶ󺸴� ���⺤���� ���̸� ���ش�. max�� ������ ������ ������ش�.
	float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	//! ����ŧ���� ���������ϴ� ���������� 1��, �ƴ� ���������� 0���� ���ǵǴ� ����ŧ���� ���Ⱑ �ʿ��ϴ�.
	vector		vLook = In.vWorldPos - g_vCamPosition; //! �ü� ����

	//TODO �ݻ纤�͸� �����ִ� �Լ��� �۵�����
	//! �ȼ��� ���������� ���̴� ��� 1�� �����س�����.
	//! ���� �ȼ��� �鿡 ������ ���� ���⺤�͸� �ݴ�� �����´�. �� �ݴ�� ������ ���͸� �����⺤�Ͷ�� ǥ���ϰڴ�.
	//! �����⺤�Ϳ� �������͸� �����ϸ� ���翵�������� ���̰� ���´�.
	//! 1�� ���̸� ���� �������Ϳ��� ���翵���̸� �����ָ� ���������� ���⺤�Ϳ� ���� ���̴� ���翵������ ���Ͱ� ��������µ� �� ���͸� ������Ͷ� �ϰڴ�.
	//! ��� ���͸� �����Ͱ��ƴ� ���� ���� ���� ���⺤�Ϳ��� �����ָ� �����̵����Ͱ� ������, �ѹ� �� �����ָ� �ݻ纤�Ͱ� ��������.
	//! �� ������ �Ʒ� reflect �Լ����� ���ְ��ִ� ��.
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	//TODO �� �ݻ纤�Ϳ� �ü����͸� �̿��� ���������� ���� ������� ����ŧ�� ����
	//! pow(�������Լ�)�� ����� ������ ����ŧ�� ���� �׷����� ���ġ�� �ʹ� ���Գ����⶧���� ���������༭ ������ ��.
	float	fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt;

	return Out;
}



technique11 DefaultTechnique //! �ٷ�9 ���ķ� ��ũ��ũ�ڿ� ������ �ٿ������. �츰 �ٷ�11�̴� 11�� �ٿ���
{
	pass Terrain
	{
		//! ����������Ʈ�� �ü��� ����.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	/* ���� �ٸ� ���¿� ���� ���ϴ� Ư�� ���̴����� �׸��� �𵨿� �����Ѵ�. */
	pass Delete
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}
};