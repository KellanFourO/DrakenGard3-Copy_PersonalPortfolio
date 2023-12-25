#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Engine_Function.h"

//TODO 컴포넌트
#include "RigidBody.h"
#include "StateMachine.h"

//TODO 파츠
#include "PlayerPart_Body.h"
#include "PlayerPart_Weapon.h"

//TODO 상태
//#include "PlayerState_Idle.h"
//#include "PlayerState_Walk.h"
//#include "PlayerState_Run.h"
//#include "PlayerState_Jump.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CAnimObject(rhs)
{
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

	//m_pRigidBodyCom->Tick(fTimeDelta);
	m_pStateCom->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	m_pStateCom->Tick(fTimeDelta);

	Key_Input(fTimeDelta);

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	m_pStateCom->Late_Tick(fTimeDelta);
	//m_fAccTime += fTimeDelta;

	//if (m_fAccTime > 0.2f)
	//{
	//	printf(Engine::ConvertWstrToStr(m_pCurrentState->Get_Name()).c_str());
	//	printf("\n\n");
	//	m_fAccTime = 0.f;
	//}

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

	//TODO 상태머신
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateCom))))
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

	//if(FAILED(Add_State(TEXT("PlayerState_Idle"), CPlayerState_Idle::Create(this))))
	//	return E_FAIL;
	//
	//if (FAILED(Add_State(TEXT("PlayerState_Walk"), CPlayerState_Walk::Create(this))))
	//	return E_FAIL;
	//
	//if (FAILED(Add_State(TEXT("PlayerState_Run"), CPlayerState_Run::Create(this))))
	//	return E_FAIL;
	//
	//if (FAILED(Add_State(TEXT("PlayerState_Jump"), CPlayerState_Jump::Create(this))))
	//	return E_FAIL;


	//if(FAILED(Set_CurrentState(TEXT("PlayerState_Idle"))))
	//	return E_FAIL;

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

void CPlayer::Key_Input(const _float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_TAB))
		m_bAdmin = !m_bAdmin;

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

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);
	
		m_PartObjects.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pNavigationCom);
}

