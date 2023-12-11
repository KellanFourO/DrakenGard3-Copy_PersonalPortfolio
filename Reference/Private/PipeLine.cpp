#include "PipeLine.h"
#include "GameInstance.h"

CPipeLine::CPipeLine()
	: m_pGameInstace(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstace);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_Transform[eState], TransformMatrix);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	m_Transform[eState] = TransformMatrix;
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eState)
{
	return m_Transform[eState];
}

_matrix CPipeLine::Get_TransformMatrixInverse(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform_Inverse[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4Inverse(D3DTRANSFORMSTATE eState)
{
	return m_Transform_Inverse[eState];
}

RAY CPipeLine::Get_Ray(_uint& In_ViewPortWidth, const _uint& In_ViewPortHeight)
{
	HWND hWnd = m_pGameInstace->Get_GraphicDesc().hWnd;


	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vector vProjPos(XMVectorSet(ptMouse.x / (In_ViewPortWidth * 0.5f) - 1.f, ptMouse.y / -(In_ViewPortHeight * 0.5f) + 1.f, 0.f, 0.f));
	
	_matrix matProjInv(Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ));
	
	_vector vViewPos(XMVector3TransformCoord(vProjPos, matProjInv));

	_vector vRayDir(vViewPos);
	_vector vRayPos(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_matrix matViewInv(Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW));
	
	RAY MouseRay = {};

	XMStoreFloat3(&MouseRay.vDirection, XMVector3Normalize(XMVector3TransformNormal(vRayDir, matViewInv)));
	XMStoreFloat4(&MouseRay.vOrigin, XMVector3TransformCoord(vRayPos, matViewInv));

	MouseRay.fLength = 1000000.0f;

	return MouseRay;
}

_float4 CPipeLine::Get_CamPosition()
{
	return m_vCamPosition;
}

HRESULT CPipeLine::Initialize()
{
	//! 루프 돌아서 항등행렬로 초기화
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}
	
	return S_OK;
}

void CPipeLine::Tick()
{
	//! 매 틱마다 카메라의 역행렬(월드행렬) 멤버변수 채워주기
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Transform[i])));
	}

	//! 위에서 구한 카메라의 월드행렬의 위치에 해당하는 4행을 위치 멤버변수에 셋팅해준다.
	memcpy(&m_vCamPosition,&m_Transform_Inverse[D3DTS_VIEW].m[3], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPipeLine::Free()
{
	Safe_Release(m_pGameInstace);
}
