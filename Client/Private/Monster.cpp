#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "Player.h"
#include "Model.h"
#include "Animation.h"
#include "PartObject.h"

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
	m_tStatus.fMaxHp = fMaxHp;
	m_tStatus.fDmg = fDmg;
	m_tStatus.eAttackType = tagStatusDesc::ATTACKTYPE_END;
	m_tOriginStatus = m_tStatus;

}


_float4 CMonster::Create_RandomPosition(_float3 vCurrentPosition, _float fMinDistance, _float fMaxDistance)
{
	std::mt19937 generator(static_cast<unsigned int>(std::time(0))); // ���� ������ �ʱ�ȭ
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f); // 0.0���� 1.0 ������ ���� ����

	// ������ ����� �Ÿ� ����
	float randomAngle = 2.0f * XM_PI * distribution(generator); // 0���� 2*pi ������ ������ ����
	float randomDistance = fMinDistance + (fMaxDistance - fMinDistance) * distribution(generator);
	float randomZ = fMinDistance + (fMaxDistance - fMinDistance) * distribution(generator); 

	// ������ ��ġ ����
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

void CMonster::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	
}

void CMonster::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
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
