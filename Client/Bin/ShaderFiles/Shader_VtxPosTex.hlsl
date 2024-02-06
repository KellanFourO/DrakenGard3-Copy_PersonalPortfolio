#include "Shader_Defines.hlsli"
//TODO 셰이더에서의 Vector
//!셰이더에서의 Vector는 float2, float3, float4 로 사용할 수 있다. 그 중에서도 float4 자료형이 vector와 같다.
//! vector.x 는 vector.r과 같다.
//! x,y,z,w == r,g,b,a 처럼 사용할 수 있으나 사용하기에 따라 해석하는 의미는 달라질 수 있다.
//! vector.xy = 1.f <-- 셰이더에서의 vector는 한번에 여러 값을 변경할 수 있다. x,y는 1로 값이 바뀐 것

//TODO 셰이더에서의 Matrix
//! 셰이더에서의 Matrix는 float4x4와 같다. 마찬가지로 vector처럼 여러 값을 채울수 있으나 잘 쓰이지 않는다.

//TODO 클라이언트로부터 받은 정점의 정보를 마찬가지로 클라이언트에서 넘겨받은 월드, 뷰, 투영행렬을 담기 위한 전역변수
//! 셰이더에서의 전역변수는 상수테이블(Constant Table)이라고도 불리운다. 
//! 말 그대로, 상수화 되어서 값을 사용은 가능하나 변경은 불가능하다. 그래서 클라이언트에서 던져주는 것.
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

//TODO 셰이더가 하는 일
//! 정점의 변환 ( 월드변환, 뷰변환, 투영변환 ) 을 수행한다. ( 뷰행렬의 투영행렬을 곱했다고 투영 스페이스에 있는 것이아니다. 반드시 w나누기 까지 거친 다음에야 투영 스페이스 변환이 됐다고 할 수 있다. )
//! 정점의 구성정보를 추가, 삭제 등의 변경을 수행한다.

struct VS_IN
{
	float3		vPosition : POSITION; //! 위치값을 나타내는 시맨틱, 아직 연산을 해야하는 자료형임을 알림.
	float2		vTexCoord : TEXCOORD0; //! 텍스처의 좌표값을 나타내는 시맨틱, UV 좌표계는 0 ~ 1사이의 값을 가지는 데, 특정 위치에 색상값을 알기위함?
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; //! 리턴해주기위한 위치. 렌더링파이프라인을 수행 후 w값이 있는채여야해서 float4로 바뀌었고, SV_POSITION 시맨틱을 줘서 연산이 끝난 자료형이란 것을 셰이더에게 알림
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
	float2		vTexCoord :	TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! 다렉9에서는 COLOR라고 했었으나 11에서 바뀌었다. 훨씬 가독성이 좋아졌다고 할 수 있다. 왜? 몇번째 렌더타겟에 그릴꺼냐 라는 직관적이니까
};

//TODO 픽셀 셰이더 : 픽셀의 색!!!!!을 결정한다.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    vector vSourColor = g_Texture[0].Sample(LinearSampler, In.vTexCoord);
    vector vDestColor = g_Texture[1].Sample(LinearSampler, In.vTexCoord);

	Out.vColor = vSourColor + vDestColor; //! 아직 텍스처로드 못했으니 그냥 빨간색으로 채움.
	
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
    
	/* In.vPosition * 월드 * 뷰 * 투영 */
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
    
	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexCoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}



/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
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


/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
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

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
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

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
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
        vColor = float4(1, 0, 0, 1); // 빨
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // 노
    else;

    if (vDissolve.r >= sinTime - 0.025 && vDissolve.r <= sinTime + 0.025)
        vColor = float4(1, 1, 1, 1); // 흰
    else;

    Out.vColor = vColor;
    
    if (0 == Out.vColor.a)
        discard;
   
    return Out;
};


technique11 DefaultTechnique //! 다렉9 이후로 테크니크뒤에 버전을 붙여줘야함. 우린 다렉11이니 11로 붙여줌
{
	pass UI // 0번 패스
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

	/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle //1번 패스
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
    pass Effect // 2번 패스
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

    pass Atlas_Default //3번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS();
    }

    pass Atlas_Grid // 4번 패스
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_GRID();
    }


    pass Atlas_Animation //5번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION();
    }

    pass Effect_Trail //6번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_TRAIL();
    }


    pass UI_HP //7번 패스
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_HP();
    }

    pass UI_HPFrame //8번 패스
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_HPFrame();
    }

    pass Atlas_Animation_TimeAlpha //9번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION_TIMEALPHA();
    }

    pass Atlas_Animation_UseMask_UseNoise //10번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ANIMATION_USEMASK_USENOISE();
    }

    pass Effect_UseMask // 11번 패스
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

    pass Effect_UseMask_UseNoise // 12번 패스
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

    pass DissolveBasic // 13번 패스
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

    pass DissolvePlus // 14번 패스
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