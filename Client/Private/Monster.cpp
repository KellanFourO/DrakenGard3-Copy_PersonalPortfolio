#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "Player.h"
#include "Model.h"
#include "Animation.h"
#include "PartObject.h"
#include "UI_MonsterHP.h"
#include "UI_MonsterHPFrame.h"
#include "Effect_Trail.h"
#include "UI_MonsterTag.h"
#include "UI_MonsterTagFrame.h"
#include "UI_MonsterPortrait.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice,pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CAnimObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize_UI(MONSTERTYPE eType)
{
	CUI_MonsterHP::MONSTER_HP HP_Desc;

	if (m_isBoss == true)
	{
		HP_Desc.bWorldUI = false;
		HP_Desc.eMonsterType = CUI_MonsterHP::BOSS;
		HP_Desc.fSizeX = 1024.f;
		HP_Desc.fSizeY = 16.f;
		HP_Desc.fX = g_iWinSizeX / 2;
		HP_Desc.fY = 60.f;
		HP_Desc.bEnable = true;

	}
	else
	{
		HP_Desc.bWorldUI = true;
		HP_Desc.eMonsterType = CUI_MonsterHP::COMMON;
	}

	//!주석 걸린 값들은 디폴트 인자. 변경 필요할시 값 변경
	//HP_Desc.fCrntHPUV = 1.f;
	//HP_Desc.fPrevHPUV = 1.f;
	//HP_Desc.fPosOffset = 0.5f;
	//HP_Desc.fOffsetX
	HP_Desc.pOwnerStatus = &m_tStatus;
	HP_Desc.pOwnerTransform = m_pTransformCom;
	
	if (m_isBoss == true)
	{
		CUI_MonsterHP::MONSTER_HP BOSS_FrameDesc;
		
		BOSS_FrameDesc = HP_Desc;
		BOSS_FrameDesc.fSizeY = 32.f;


		if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterHPFrame"), &BOSS_FrameDesc)))
			return E_FAIL;
	}


	if (m_isBoss == true)
	{
		CUI_MonsterTag::MONSTER_TAG MonsterTagDesc;
		MonsterTagDesc.bWorldUI = false;
		MonsterTagDesc.eMonsterType = (CUI_MonsterTag::MONSTERTYPE)eType;
		MonsterTagDesc.pOwnerStatus = &m_tStatus;
		MonsterTagDesc.pOwnerTransform = m_pTransformCom;
		MonsterTagDesc.fSizeX = 300.f;
		MonsterTagDesc.fSizeY = 37.5f;
		MonsterTagDesc.fX = 340.f;
		MonsterTagDesc.fY = 25.f;
		MonsterTagDesc.bEnable = true;


		if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterNameTag"), &MonsterTagDesc)))
			return E_FAIL;

		CUI_MonsterPortrait::MONSTER_PORTRAIT MonsterPortraitDesc;
		MonsterPortraitDesc.bWorldUI = false;
		MonsterPortraitDesc.eMonsterType = (CUI_MonsterPortrait::MONSTERTYPE)eType;
		MonsterPortraitDesc.pOwnerStatus = &m_tStatus;
		MonsterPortraitDesc.pOwnerTransform = m_pTransformCom;
		MonsterPortraitDesc.fSizeX = 200.f;
		MonsterPortraitDesc.fSizeY = 200.f;
		MonsterPortraitDesc.fX = 100.f;
		MonsterPortraitDesc.fY = 60.f;
		MonsterPortraitDesc.bEnable = true;

		if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterPortrait"), &MonsterPortraitDesc)))
			return E_FAIL;
	}
	else
	{
		CUI_MonsterTag::MONSTER_TAG MonsterTagDesc;
		MonsterTagDesc.bWorldUI = true;
		MonsterTagDesc.eMonsterType = (CUI_MonsterTag::MONSTERTYPE)eType;
		MonsterTagDesc.pOwnerStatus = &m_tStatus;
		MonsterTagDesc.pOwnerTransform = m_pTransformCom;


		if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterNameTag"), &MonsterTagDesc)))
			return E_FAIL;

		CUI_MonsterPortrait::MONSTER_PORTRAIT MonsterPortraitDesc;
		MonsterPortraitDesc.bWorldUI = true;
		MonsterPortraitDesc.eMonsterType = (CUI_MonsterPortrait::MONSTERTYPE)eType;
		MonsterPortraitDesc.pOwnerStatus = &m_tStatus;
		MonsterPortraitDesc.pOwnerTransform = m_pTransformCom;

		if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterPortrait"), &MonsterPortraitDesc)))
			return E_FAIL;

	}
		


	
	
	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterHP"), &HP_Desc)))
		return E_FAIL;
	
		


	

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
}

void CMonster::Tick(_float fTimeDelta)
{
}

void CMonster::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	
	return S_OK;
}

void CMonster::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);




}

void CMonster::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);
}

void CMonster::Init_Desc()
{
}

void CMonster::Init_Status(_float fMaxHp, _float fDmg)
{
	m_tStatus.fCurrentHp = fMaxHp;
	m_tStatus.fMaxHp = fMaxHp;
	m_tStatus.fDmg = fDmg;
	m_tStatus.eAttackType = tagStatusDesc::ATTACKTYPE_END;
	m_tOriginStatus = m_tStatus;

}


_float4 CMonster::Create_RandomPosition(_float3 vCurrentPosition, _float fMinDistance, _float fMaxDistance)
{
	std::mt19937 generator(static_cast<unsigned int>(std::time(0))); // 난수 생성기 초기화
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f); // 0.0에서 1.0 사이의 균일 분포

	// 랜덤한 방향과 거리 생성
	float randomAngle = 2.0f * XM_PI * distribution(generator); // 0에서 2*pi 사이의 랜덤한 각도
	float randomDistance = fMinDistance + (fMaxDistance - fMinDistance) * distribution(generator);
	float randomZ = fMinDistance + (fMaxDistance - fMinDistance) * distribution(generator); 

	// 랜덤한 위치 생성
	XMFLOAT4 randomPosition;
	randomPosition.x = vCurrentPosition.x + randomDistance * cosf(randomAngle);
	randomPosition.y = vCurrentPosition.y;
	randomPosition.z = vCurrentPosition.z + randomZ;
	randomPosition.w = 1.f;

	return randomPosition;
	
}

_bool CMonster::Is_CurrentAnimEnd()
{
	return m_pModelCom->Get_CurrentAnimation()->Get_Finished();
	
}

void CMonster::Set_AnimSpeed(_float fSpeed)
{
	m_pModelCom->Set_AnimationSpeed(fSpeed);
}

CCollider* CMonster::Get_WeaponCollider()
{
	return dynamic_cast<CCollider*>(Find_PartObject(TEXT("Part_Weapon"))->Find_Component(TEXT("Com_Collider")));
}

void CMonster::Set_WeaponAttackType(STATUS_DESC::ATTACKTYPE eAttackType)
{
	Find_PartObject(TEXT("Part_Weapon"))->Get_Status()->eAttackType = eAttackType;
}

void CMonster::Set_PartAttackType(wstring& strPartTag, STATUS_DESC::ATTACKTYPE eAttackType)
{
	Find_PartObject(strPartTag)->Get_Status()->eAttackType = eAttackType;
}



void CMonster::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	
}

void CMonster::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	if (bType == false)
	{
		m_bMove = false;
	}
	else if(bHit == true)
	{
		if (typeid(*pCollisionObject) == typeid(CPlayer))
		{
			
			Blackboard::Ptr pBlackBoard = m_pBehaviorTree->getBlackboard();
			
			_float fCurrentHp = pBlackBoard->getFloat("Current_HP");
			_float fCalcHp = fCurrentHp - 50.f;


			if (fCalcHp <= 0)
			{
				pBlackBoard->setBool("Is_Dead", true);
			}
			else
				pBlackBoard->setBool("Is_Hit", true);
				
			m_tStatus.fCurrentHp = fCalcHp;
			pBlackBoard->setFloat("Current_HP", fCalcHp);
		}
	}

	
	

	wcout << LeftTag.c_str() << TEXT(" On_CollisionExit is ") << RightTag.c_str() << endl;
}

void CMonster::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	if (bType == false)
	{
		m_bMove = true;
	}
	//else if(bHit == true)
	//{
	//	if (typeid(*pCollisionObject) == typeid(CPlayer))
	//	{
	//		m_pBehaviorTree->getBlackboard()->setBool("Is_Hit", false);
	//		
	//	}
	//}
	

	wcout << LeftTag.c_str() << TEXT(" On_CollisionExit is ") << RightTag.c_str() << endl;
}

void CMonster::Transition(_int iAnimIndex, _float fSpeed)
{
	if (iAnimIndex != m_pModelCom->Get_CurrentAnimationIndex())
	{
		
		m_pModelCom->Set_Animation(iAnimIndex);
		m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
		m_pModelCom->Set_AnimationSpeed(fSpeed);
	}
}


HRESULT CMonster::Ready_BehaviorTree()
{
	return S_OK;
}

HRESULT CMonster::Ready_BehaviorTree_V2()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
