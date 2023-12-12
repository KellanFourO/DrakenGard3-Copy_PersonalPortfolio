
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector      g_vColor = vector(1.f, 1.f, 1.f, 1.f); 

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0; //! HLSL에서만 사용가능한 구조체 제로메모리
	
	//! 매개인자로 들어온 In은 로컬 영역이다
	//! In.vPosition * 월드 * 뷰 * 투영을 해줘야한다.
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

	//TODO mul 함수는 행렬곱이 가능한 모든 행렬들의 곱셈을 수행해준다. 

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); //! 클라에서 던져준 정점의 위치정보를 행렬곱이 가능하게 행을 맞춰주고 w값이니 1로 초기화 시켜준 행렬과 월드,뷰,투영 곱을 마친 행렬을 곱해준다.
	
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

//TODO 픽셀 셰이더 : 픽셀의 색!!!!!을 결정한다.
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_vColor;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass Cell
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
};