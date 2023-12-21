#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "Engine_Function.h"
//TODO 파츠
#include "PlayerPart_Body.h"
#include "PlayerPart_Weapon.h"

//TODO 상태
#include "StateMachine.h"
#include "PlayerState_Idle.h"
#include "PlayerState_Walk.h"
#include "PlayerState_Run.h"
#include "PlayerState_Jump.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CAnimObject(rhs)
{
}

HRESULT CPlayer::Set_CurrentState(const wstring& strStateTag)
{
	CStateMachine* pStateMachine = Find_States(strStateTag);

	if(nullptr == pStateMachine)
		return E_FAIL;


	//if(nullptr != m_pCurrentState && FAILED(m_pCurrentState->Replaceability(pStateMachine)))
	//	return S_OK;

	if(nullptr != m_pCurrentState)
		static_cast<CPlayerState_Base*>(m_pCurrentState)->ResetState();

	m_pCurrentState = pStateMachine;

	CPlayerPart_Body* pBody = static_cast<CPlayerPart_Body*>(Find_PartObject(TEXT("Part_Body")));
	Safe_AddRef(pBody);
	
	pBody->SetUp_Animation(m_pCurrentState->Get_AnimIndex());

	Safe_Release(pBody);

	return S_OK;
}

HRESULT CPlayer::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{	
	CGameObject::GAMEOBJECT_DESC PlayerDesc = {};

	PlayerDesc.fSpeedPerSec = 10.0f;
	PlayerDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&PlayerDesc)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	//m_pModelCom->Set_Animation(3);

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	if (!m_bAdmin)
	{
		m_pCurrentState->Priority_Tick(fTimeDelta);
	}

	//m_pRigidBodyCom->Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{

	if (!m_bAdmin)
	{
		m_pCurrentState->Tick(fTimeDelta);
	}
	

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	Key_Input(fTimeDelta);

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (!m_bAdmin)
	{
		m_pCurrentState->Late_Tick(fTimeDelta);
	}

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	m_fAccTime += fTimeDelta;

	if (m_fAccTime > 0.2f)
	{
		printf(Engine::ConvertWstrToStr(m_pCurrentState->Get_Name()).c_str());
		printf("\n\n");
		m_fAccTime = 0.f;
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return ;
}

HRESULT CPlayer::Render()
{
	
	#ifdef _DEBUG
		m_pNavigationCom->Render();
		m_pColliderCom->Render();
	#endif


	return S_OK;
}

CPartObject* CPlayer::Find_PartObject(const wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);

	if(iter == m_PartObjects.end())
		return nullptr;
	
	return iter->second;
}

CStateMachine* CPlayer::Find_States(const wstring& strStateTag)
{
	auto iter = m_States.find(strStateTag);

	if (iter == m_States.end())
		return nullptr;

	return iter->second;
}


HRESULT CPlayer::Ready_Components()
{

	/* For.Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	//TODO 리지드바디
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	//! For.Part_Body 
	CPartObject::PART_DESC BodyDesc = {};

	BodyDesc.m_pParentTransform = m_pTransformCom;
	if(FAILED(Add_PartObject(TEXT("Prototype_PartObject_PlayerBody"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	//! For.Part_Weapon
	CPartObject::PART_DESC WeaponDesc = {};

	WeaponDesc.m_pParentTransform = m_pTransformCom;

	CPlayerPart_Body* pBody = dynamic_cast<CPlayerPart_Body*>(Find_PartObject(TEXT("Part_Body")));

	//TODO  내 모델 뼈 이름 찾아서 수정하자

	CBone* pSwordBone = pBody->Get_BonePtr("R_FINGER52");
	
	if(nullptr == pSwordBone)
		return E_FAIL;
	
	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;
	
	
	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_PlayerWeapon"), TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::Ready_States()
{

	if(FAILED(Add_State(TEXT("PlayerState_Idle"), CPlayerState_Idle::Create(this))))
		return E_FAIL;

	if (FAILED(Add_State(TEXT("PlayerState_Walk"), CPlayerState_Walk::Create(this))))
		return E_FAIL;

	if (FAILED(Add_State(TEXT("PlayerState_Run"), CPlayerState_Run::Create(this))))
		return E_FAIL;

	if (FAILED(Add_State(TEXT("PlayerState_Jump"), CPlayerState_Jump::Create(this))))
		return E_FAIL;


	if(FAILED(Set_CurrentState(TEXT("PlayerState_Idle"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CPlayer::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if(nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Get_CloneObject(strPrototypeTag, pArg);
	
	if(nullptr == pPartObject)
		return E_FAIL;
	
	m_PartObjects.emplace(strPartTag, dynamic_cast<CPartObject*>(pPartObject));
	
	return S_OK;
}

HRESULT CPlayer::Add_State(const wstring& strStateTag, CStateMachine* pStateMachine)
{
	if(nullptr == pStateMachine)
		return E_FAIL;

	if(nullptr != Find_PartObject(strStateTag))
		return E_FAIL;

	m_States.emplace(strStateTag, pStateMachine);
	
	return S_OK;
}

void CPlayer::Key_Input(const _float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_TAB))
		m_bAdmin = !m_bAdmin;

	if (!m_bAdmin)
	{
		if (m_pGameInstance->Key_Down(DIK_F3))
		{
			m_iCurrentAnimIndex++;
			m_pCurrentState->Set_CompulsionChangeAnim(m_iCurrentAnimIndex);
		}

		if (m_pGameInstance->Key_Down(DIK_F4))
		{
			m_iCurrentAnimIndex--;
			m_pCurrentState->Set_CompulsionChangeAnim(m_iCurrentAnimIndex);
		}
			

		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}
		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
		if (GetKeyState(VK_UP) & 0x8000)
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}
	}
}


CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for(auto& Pair : m_States)
		Safe_Release(Pair.second);

	m_States.clear();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);
	
		m_PartObjects.clear();

	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pNavigationCom);
}

