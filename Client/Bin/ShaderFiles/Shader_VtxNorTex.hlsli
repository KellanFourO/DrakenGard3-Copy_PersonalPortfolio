
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f); //! 원래는 클라에서 던져줘야한다. 임의로 정한 빛의 방향 벡터
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f); //! 임의로 정한 빛의 색

//TODO 엠비언트 컬러는 전역으로 쓸것이다.
//! 엠비언트 색상은 빛을 전혀 받지못하는 부분이 너무 어둡게 그려지면 오히려 현실성이 떨어지므로 보간하기위한 색상을 정의한다
//! 현재 셰이더를 사용하는 모든 객체는 아래에서 정의한 엠비언트 컬러에게 영향을 받는다고 보면된다.
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f); //! 반사엠비언트

//TODO 텍스처는 빛연산에 필요한 픽셀의 디퓨즈색상을 전달받기위한 수단이다. 그래서 변수명을 재정의했다
//!texture2D		g_Texture;
texture2D		g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR; //! 리니어로 주어도 밉레벨이 없는 텍스처형태( dx11에서는 dds만 가능)가 들어왔다면 리니어를 줘도 적용되지 않는다.
	AddressU = wrap;
	AddressV = wrap;
};

//TODO 셰이더가 하는 일
//! 정점의 변환 ( 월드변환, 뷰변환, 투영변환 ) 을 수행한다. ( 뷰행렬의 투영행렬을 곱했다고 투영 스페이스에 있는 것이아니다. 반드시 w나누기 까지 거친 다음에야 투영 스페이스 변환이 됐다고 할 수 있다. )
//! 정점의 구성정보를 추가, 삭제 등의 변경을 수행한다.

struct VS_IN
{
	float3		vPosition : POSITION; //! 위치값을 나타내는 시맨틱, 아직 연산을 해야하는 자료형임을 알림.
	float3		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0; //! 텍스처의 좌표값을 나타내는 시맨틱, UV 좌표계는 0 ~ 1사이의 값을 가지는 데, 특정 위치에 색상값을 알기위함?
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! 리턴해주기위한 위치. 렌더링파이프라인을 수행 후 w값이 있는채여야해서 float4로 바뀌었고, SV_POSITION 시맨틱을 줘서 연산이 끝난 자료형이란 것을 셰이더에게 알림
	float3		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; //! HLSL에서만 사용가능한 구조체 제로메모리
	
	//! 매개인자로 들어온 In은 로컬 영역이다
	//! In.vPosition * 월드 * 뷰 * 투영을 해줘야한다.
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//TODO mul 함수는 행렬곱이 가능한 모든 행렬들의 곱셈을 수행해준다. 

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); //! 클라에서 던져준 정점의 위치정보를 행렬곱이 가능하게 행을 맞춰주고 w값이니 1로 초기화 시켜준 행렬과 월드,뷰,투영 곱을 마친 행렬을 곱해준다.
	//! 노말은 방향벡터니까 w값이 필요없어서 0. 빛의 방향벡터는 월드좌표계에 있다. 후에 노말과 빛의 방향벡터를 가지고 만들어내는 연산이 있는데 같은 좌표계로 맞춰주기위해 In.vNormal을 월드좌표계로 올린다.
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexCoord = In.vTexCoord; //! 텍스쿠드는 변하지 않는다는 것을 알 수 있다.
	
	return Out;
}

//TODO 순서
//! 정점을 그리는 건 결국 인덱스로 그릴거다.
//! 사각형을 예시로 들자면  정점 4개, 인덱스 6개를 가지고 있다. 정점은 { 0,1,2,3 } 인덱스는 { {0,1,2}, {0,2,3} }을 가지고 있다.
//! 0번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 1번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 2번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 이제 통과된 정점의 개수가 3개가 되었으니 그리는 작업을 시작한다 ( Why? 밖에서 트라이앵글리스트로 그린다고 했으니까 ( 삼각형으로 그린다 ) )
//! 정점 3개에 대해서 w나누기 변환, 뷰포트 변환, 이제 정점 세 개의 안을 막 채운다는 작업흐름이 이어짐.
//! 정점셰이더는 6번 통과
//! 픽셸셰이더는 가로 사이즈 * 세로 사이즈 수행 800 * 600 일 때 = 480,000 번 호출

struct PS_IN
{
	float4		vPosition : SV_POSITION; //! 왜 PS_IN인데 연산이 끝낫다는 시맨틱으로? 래스터라이즈 과정을 거쳤다고 하면 당연히 픽셀화가 된 이후이니 연산이 끝났다.
	float3		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! 다렉9에서는 COLOR라고 했었으나 11에서 바뀌었다. 훨씬 가독성이 좋아졌다고 할 수 있다. 왜? 몇번째 렌더타겟에 그릴꺼냐 라는 직관적이니까
};

//TODO 픽셀 셰이더 : 픽셀의 색!!!!!을 결정한다.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	//!이것은 다시 정의해야한다. 내보내는 최종 컬러값이 아닌 텍스처로부터 읽어온 반사할 Diffuse 색상을 의미한다.
	//Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	//! 다시 정의된 변수
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

	return Out;
}

technique11 DefaultTechnique //! 다렉9 이후로 테크니크뒤에 버전을 붙여줘야함. 우린 다렉11이니 11로 붙여줌
{
	pass UI
	{
		//! 렌더스테이트가 올수도 있음.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};