#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Engine_Function.h"


//TODO ������Ʈ

//TODO ����
#include "PlayerPart_Body.h"
#include "PlayerPart_Weapon.h"

//TODO ����
#include "PlayerGroundStates.h"

#include "Camera_Target.h"

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
	m_isPlayer = true;
	

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

	if(FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	m_pStateCom->Priority_Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_pStateCom->Tick(fTimeDelta);
	
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	Key_Input(fTimeDelta);

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pStateCom->Late_Tick(fTimeDelta);
	
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}


	m_fAccTime += fTimeDelta;

	if (m_fAccTime > 0.2f)
	{
		//printf(m_pStateCom->Get_aNI);
		printf(ConvertWstrToStrTest(m_pStateCom->Get_StateTag()).c_str());
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

void CPlayer::Set_Cam(CCamera_Target* pCam)
{
	m_pCamera = pCam;
	Safe_AddRef(m_pCamera);
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
	CBoundingBox_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	//TODO ������ٵ�
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), m_pTransformCom)))
		return E_FAIL;

	///TODO ���¸ӽ�
	//f (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
	//	TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateCom), this)))
	//	return E_FAIL;
	
	//TODO ���¸ӽ�
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateCom), this)))
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

	//TODO  �� �� �� �̸� ã�Ƽ� ��������

	CBone* pSwordBone = pBody->Get_BonePtr("M_WP1");
	
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

	if(FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Idle"), CPlayerState_Idle::Create(this))))
		return E_FAIL;
	
	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Walk"), CPlayerState_Walk::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Run"), CPlayerState_Run::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Jump"), CPlayerState_Jump::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack1"), CPlayerState_Attack1::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack2"), CPlayerState_Attack2::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack3"), CPlayerState_Attack3::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack4"), CPlayerState_Attack4::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack5"), CPlayerState_Attack5::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack6"), CPlayerState_Attack6::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack7"), CPlayerState_Attack7::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateCom->Add_State(TEXT("PlayerState_Attack8"), CPlayerState_Attack8::Create(this))))
		return E_FAIL;

	
	m_pStateCom->Set_InitState(TEXT("PlayerState_Idle"));
	

	return S_OK;
}

HRESULT CPlayer::Ready_Camera()
{
	CCamera_Target::TARGET_CAMERA_DESC Desc = {};

	Desc.fMouseSensitivity = 0.05f;
	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovY = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.pTarget = this;

	CGameObject* pCam = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Target"), &Desc, &pCam)))
		return E_FAIL;

	m_pCamera = dynamic_cast<CCamera_Target*>(pCam);
	Safe_AddRef(m_pCamera);

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

string CPlayer::ConvertWstrToStrTest(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
	string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &str[0], size_needed, nullptr, nullptr);
	return str;
}


CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	if (nullptr != m_pStateCom)
	{
		m_pStateCom->Clear();
	}

	Safe_Release(m_pCamera);
	Safe_Release(m_pStateCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pNavigationCom);

}

