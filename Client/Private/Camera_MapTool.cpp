#include "stdafx.h"
#include "Camera_MapTool.h"
#include "GameInstance.h"

CCamera_MapTool::CCamera_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_MapTool::CCamera_MapTool(const CCamera_MapTool& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_MapTool::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	return S_OK;
}

HRESULT CCamera_MapTool::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	DYNAMIC_CAMERA_DESC* pDesc = (DYNAMIC_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_MapTool::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_MapTool::Tick(_float fTimeDelta)
{
	KeyInput(fTimeDelta);
	
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		MouseInput(fTimeDelta);
	}

	//TODO 부모의 Tick함수를 호출해줘야 뷰투영행렬을 파이프라인 객체에게 던져준다.
	__super::Tick(fTimeDelta);
}

void CCamera_MapTool::Late_Tick(_float fTimeDelta)
{
}

void CCamera_MapTool::KeyInput(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFTARROW))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_RIGHTARROW))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_UPARROW))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_DOWNARROW))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_Q))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_E))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}

}

void CCamera_MapTool::MouseInput(_float fTimeDelta)
{
	_long	MouseMove = 0;

	//TODO 마우스 무브함수는 왼쪽으로 움직일시 음수, 오른쪽으로 움직일 시 양수를 리턴한다
	//! 이전프레임에 조사했을 때의 마우스 위치기준으로 몇픽셀이나 움직였는지의 값을 리턴한다.
	//! 무슨 축을 기준으로 회전을 할 것이냐, 0,1,0을 주는 방법과 카메라의 UP벡터를 주는 방법이있다.
	//! 0,1,0 은 수평이 고정된 상태인 것이고 , 후자의 경우는 꿀렁이는 느낌을 받을 수 있다. 해보니 토 나올거 같다
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		//m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), m_fMouseSensitivity * MouseMove * fTimeDelta);
		//! 리턴되는 값은 정수를 리턴한다. 정수에 실수인 타임델타를 곱해주면 안되니 마우스감도라는 실수를 넣어주자
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}

	//TODO 카메라 Y축 회전은 X축을 기준으로 회전하는 것이 아닌 라이트벡터 기준으로 해줘야 항상 어디를 바라보든 같은 회전을 할 수 있다.	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}
}

CCamera_MapTool* CCamera_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CCamera_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_MapTool::Clone(void* pArg)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_MapTool::Free()
{
	__super::Free();
}
