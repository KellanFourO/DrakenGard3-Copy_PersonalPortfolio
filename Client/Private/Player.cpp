#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PlayerPart_Body.h"
#include "PlayerPart_Weapon.h"

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

	//m_pModelCom->Set_Animation(3);

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{

}

void CPlayer::Tick(_float fTimeDelta)
{
	CPlayerPart_Body* pBody = dynamic_cast<CPlayerPart_Body*>(Find_PartObject(TEXT("Part_Body")));
	Safe_AddRef(pBody);

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
		pBody->SetUp_Animation(4);
	}
	else
		pBody->SetUp_Animation(3);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	Safe_Release(pBody);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return ;
}

HRESULT CPlayer::Render()
{
	
	#ifdef _DEBUG
		m_pNavigationCom->Render();
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

	CBone* pSwordBone = pBody->Get_BonePtr("SWORD");

	if(nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;


	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_PlayerWeapon"), TEXT("Part_Weapon"), &WeaponDesc)))
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

	Safe_Release(m_pNavigationCom);
}

