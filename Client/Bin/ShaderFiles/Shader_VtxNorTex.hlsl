
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightPos = vector(50.f, 10.f, 50.f, 1.f); //! 점광원 위치 벡터
float			g_fLightRange = 30.f; //! 점광원 범위

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f); //! 원래는 클라에서 던져줘야한다. 임의로 정한 빛의 방향 벡터
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f); //! 임의로 정한 빛의 색

//TODO 엠비언트 컬러는 전역으로 쓸것이다.
//! 엠비언트 색상은 빛을 전혀 받지못하는 부분이 너무 어둡게 그려지면 오히려 현실성이 떨어지므로 보간하기위한 색상을 정의한다
//! 현재 셰이더를 사용하는 모든 객체는 아래에서 정의한 엠비언트 컬러에게 영향을 받는다고 보면된다.
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);

//TODO 스페큘럼은 정반사를 표현하는 색상이다. 
//! 정반사는 빛이 맞고 제대로 튕긴다는 것을 의미한다. 
//! 그 튕긴 빛이 내 시선에 들어와야지 확인이 될 수 있는 색상. ==  정반사에 의한 색상, ex) 하이라이트
//! 같은 색상을 가진 물체더라도 여러부분을 표현하고자 할 때의 주로 정반사(스펙큘러)를 이용한다.
//! 당연히 빛에도 스펙큘러색상(일반적으로 흰색) 물체에도 스펙큘러색상(마찬가지)이 있다.
//! 빛을 받았을때 어떻게 표현할 것인가에 대한 머테리얼에도 스펙큘러 색이 존재해야한다는 것이다.
//! 살색에도 빛을 비추면 하얀색 스펙큘러가 떠야 하니까
//! 디퓨즈, 엠비언트 색상과는 상관없이 하이라이트를 어떤색으로 그릴지 자유롭게 선택할 수 있도록 만들어준 개념이다.
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f); //! 반사엠비언트
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

//TODO 텍스처는 빛연산에 필요한 픽셀의 디퓨즈색상을 전달받기위한 수단이다. 그래서 변수명을 재정의했다
//!texture2D		g_Texture;
//! 아래에서부터 배열로 텍스처를 받는 이유는 마스크 텍스처를 이용해서 두장의 지형텍스처를 섞어서 그릴것이기 때문에.
texture2D		g_DiffuseTexture[2];
texture2D		g_MaskTexture;
texture2D		g_BrushTexture;

vector			g_vCamPosition; //! 반사벡터를 만들기위해 필요한 시선벡터를 구할때 필요한 카메라의 월드위치
vector			g_vBrushPos = vector(50.f, 0.f, 20.f, 1.f);
float			g_fBrushRange = 10.0f;


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
	float4		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; //! 시선벡터를 구하려면 카메라의 월드위치에서 픽셀의 월드위치를 빼줘야하는데 현재 로컬이니까 픽셀을 월드위치로 올려줘야 한다.
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
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); //! 위치벡터니까 w값의 1로 채워준 것을 확인할 수 있다
	
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
	float4		vNormal	  : NORMAL;
	float2		vTexCoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
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

	//! 마스크텍스처로 두장의 지형텍스처를 섞어서 그릴 것이기에 다시 정의되야 된다.
	//vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexCoord * 100.0f);

	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexCoord); //! 마스크는 타일링시키지 않을것이기에 uv좌표에 100을 곱해주지 않는다.

	//TODO Brush의 색상을 바로 얻지 않는 이유
	//! 얻어온 색깔을 디퓨즈 색상에 더해줘야하는데, 내가 지정한 위치에 내가 지정한 사이즈로 그려주길 바란다.
	//! 하지만 지금 색상을 얻어서 바로 더해주면 내가 지정한 위치와 사이즈에 픽셀들이 아닌 모든 픽셀들이 브러시 텍스처로부터 
	//! 색을 얻어오는 상황이다.
	//! 그래서 if문으로 픽셀의 월드위치와 브러시의 위치,범위를 이용해서 영역안의 픽셀을 잡아주자
	vector		vBrush = vector(0.f, 0.f, 0.f, 0.f);
	
	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		//! uv좌표를 새로 잡는 이유는 : 지금은 픽셸의 전체 uv좌표로 잡혀있다 
		//! 우리는 지금 영역안에서 브러시 전체를 그리고싶은거니 새로 잡아야한다
		float2 vUV;
		
		//! 영역안의 들어왔다는 조건을 통과한 픽셀의 월드위치의 x가 356이라고 해보자
		//! 브러시의 위치.x는 400, 브러시의 범위는 50이라고 해보자
		//! 356 - 350 = 6 / 100 = 0.06
		//! 픽셀의 월드위치.x가 450이라고 해보자
		//! 450 - 350 = 100 / 100 = 1
		//! 즉 0 ~ 1 사이의 좌표가 잡히는 산술식
		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(DefaultSampler, vUV);
	}

	//!마스크 텍스처로 읽어들인 색이 하얀색, 검정색이냐에 따라 어떤텍스처의 색으로 그릴것인지 구분하기위한 산술식이다
	//! 만약 마스크의 색상이 vector(1.f, 1.f, 1.f, 1.f) 이었다면 1과 0을 곱하니 사라지고 마스크는 0이다. 1에서 0을 빼면 SourDiffuse 그대로의 색상이나오고
	//! 반대의경우에는 1 + 0 * SourDiffuse가 되니 DestDiffuse 색만 나오는 것이다.

	//TODO 만약 브러쉬 범위조건안의 들어왔다면 vBrush가 텍스처로부터 색상을 가져와서 텍스처색상을 더해준거고 아니라면 0초기화한 vBrush값이 들어가니 그대로 진행
	vector		vMtrlDiffuse = vMask * vDestDiffuse + (1.f - vMask) * vSourDiffuse + vBrush;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
	
	//! 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다.
	vector		vLook = In.vWorldPos - g_vCamPosition; //! 시선 벡터
	
	//TODO 반사벡터를 구해주는 함수의 작동원리
	//! 픽셀의 법선벡터의 길이는 모두 1로 셋팅해놨었다.
	//! 빛이 픽셀의 면에 닿으면 빛의 방향벡터를 반대로 뒤집는다. 이 반대로 뒤집은 벡터를 역방향벡터라고 표현하겠다.
	//! 역방향벡터와 법선벡터를 내적하면 정사영했을때의 길이가 나온다.
	//! 1의 길이를 가진 법선벡터에게 정사영길이를 곱해주면 법선벡터의 방향벡터와 같고 길이는 정사영길이인 벡터가 만들어지는데 이 벡터를 녹색벡터라 하겠다.
	//! 녹색 벡터를 역벡터가아닌 기존 원래 빛의 방향벡터에게 더해주면 슬라이딩벡터가 나오고, 한번 더 더해주면 반사벡터가 구해진다.
	//! 이 원리를 아래 reflect 함수에서 해주고있는 것.
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	//TODO 위 반사벡터와 시선벡터를 이용한 최종적으로 나온 결과값인 스펙큘러 세기
	//! pow(제곱근함수)를 사용한 이유는 스펙큘러 세기 그래프의 평균치가 너무 높게나오기때문에 거듭제곱을해줘서 반사되는 각을 낮춰준 행위
	float	fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
				+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}

//TODO 점광원을 위한 함수
//! 우리 현재 셰이딩으로는 점광원을 표현하기 까다롭다. 
//! 점광원 하나정도는 괜찮으나 점광원이 늘면 늘수록 프레임드랍이 일어나기 때문에 우선 하나만 해서 테스트하고
//! 나중에 다중광원은 디퍼드셰이더 기법을 통해 사용한다.
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//! 점광원은 빛의 범위를 벗어날시에 아예 빛을 받지말아야한다. 엠비언트 색상도 마찬가지이다.

	//! 점광원은 빛의 방향을 직접 구해줘야한다.
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexCoord * 100.0f);
	vector		vLightDir = In.vWorldPos - g_vLightPos; //! 픽셀의 월드위치를 바라보는 빛의 방향

	//TODO 감쇄값 (Attenuation) 구해주는 법
	//! 빛이 가까우면 감쇄값은 1 , 범위 보다 커지면 감쇄값은 0
	//! 감쇄값은 왜 구하는데? 빛과의 거리에따라 어둡거나 환한 표현을 위해 구해주는 것.
	//! 점광원의 범위에서 점광원이 픽셀을 바라보는 방향벡터의 길이를 빼준다. max로 음수는 버리게 만들어준다.
	float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;

	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	//! 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다.
	vector		vLook = In.vWorldPos - g_vCamPosition; //! 시선 벡터

	//TODO 반사벡터를 구해주는 함수의 작동원리
	//! 픽셀의 법선벡터의 길이는 모두 1로 셋팅해놨었다.
	//! 빛이 픽셀의 면에 닿으면 빛의 방향벡터를 반대로 뒤집는다. 이 반대로 뒤집은 벡터를 역방향벡터라고 표현하겠다.
	//! 역방향벡터와 법선벡터를 내적하면 정사영했을때의 길이가 나온다.
	//! 1의 길이를 가진 법선벡터에게 정사영길이를 곱해주면 법선벡터의 방향벡터와 같고 길이는 정사영길이인 벡터가 만들어지는데 이 벡터를 녹색벡터라 하겠다.
	//! 녹색 벡터를 역벡터가아닌 기존 원래 빛의 방향벡터에게 더해주면 슬라이딩벡터가 나오고, 한번 더 더해주면 반사벡터가 구해진다.
	//! 이 원리를 아래 reflect 함수에서 해주고있는 것.
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	//TODO 위 반사벡터와 시선벡터를 이용한 최종적으로 나온 결과값인 스펙큘러 세기
	//! pow(제곱근함수)를 사용한 이유는 스펙큘러 세기 그래프의 평균치가 너무 높게나오기때문에 제곱을해줘서 보간한 것.
	float	fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt;

	return Out;
}



technique11 DefaultTechnique //! 다렉9 이후로 테크니크뒤에 버전을 붙여줘야함. 우린 다렉11이니 11로 붙여줌
{
	pass Terrain
	{
		//! 렌더스테이트가 올수도 있음.
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Delete
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}
};