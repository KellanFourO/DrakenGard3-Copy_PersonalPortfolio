#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);

texture2D		g_MaskTexture;
texture2D		g_BrushTexture;

vector			g_vCamPosition;
vector			g_vBrushPos;
float			g_fBrushRange;

struct VS_IN
{
	float3		vPosition : POSITION; 
    float3		vNormal   :	NORMAL;
    float2		vTexCoord : TEXCOORD0;
    float3		vTangent  :	TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION; 
    float4		vNormal   : NORMAL;
    float2		vTexCoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
	
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; 
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexCoord = In.vTexCoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	
	return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal	 : NORMAL;
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; 
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexCoord);
    vector vBrush = vector(0.f, 0.f, 0.f, 0.f);
	
    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vUV;

        vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

        vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
    }
	
    Out.vColor = g_vColor;
	
	return Out;
}

technique11 DefaultTechnique 
{
	pass FIELD
	{
        SetRasterizerState(RS_Fill_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}


};