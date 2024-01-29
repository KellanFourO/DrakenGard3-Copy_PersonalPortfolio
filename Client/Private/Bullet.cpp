#include "..\Public\Bullet.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& rhs)
	: CNonAnimObject(rhs)
{
}

HRESULT CBullet::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	CBullet::BULLET_DESC Desc = {};

	if (nullptr != pArg)
	{
		Desc = *(BULLET_DESC*)pArg;

		m_fDmg = Desc.fDmg;
		m_fLifeTime = Desc.fDeadTime;
		m_fRange = Desc.fRange;
		m_OwnerWorldMatrix = Desc.OwnerWorldMatrix;
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_eCurrentLevelID));


	
	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] += 1.f;
	XMStoreFloat4(&m_vTargetPos, vPlayerPos);
	
	return S_OK;
}

void CBullet::Tick(_float TimeDelta)
{
	//Go_Straight(TimeDelta);
	//Dead_Range(m_fRange);
}

void CBullet::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBullet::Render()
{
	return S_OK;
}

void CBullet::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
}

void CBullet::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	if (true == m_bHitDead)
	{
		m_fLifeTime = 0.f;
		Die(m_fLifeTime);
	}
	
}

void CBullet::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
}

void CBullet::Go_Straight(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CBullet::Go_Parabolic(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos.m128_f32[0] = m_vPrevPos.x + m_vVelocity.x * m_fTimeAcc;
	vPos.m128_f32[1] = m_vPrevPos.y + (m_vVelocity.y * m_fTimeAcc) - (0.5f * m_fGravitionalConstant * m_fTimeAcc * m_fTimeAcc);
	vPos.m128_f32[2] = m_vPrevPos.z + m_vVelocity.z * m_fTimeAcc;

	m_pTransformCom->Look_At(XMLoadFloat4(&m_vTargetPos));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	//m_pTransformCom->Go_Parabola(m_vVelocity, m_fGravitionalConstant, fTimeDelta);


	//m_fTimeAcc += fTimeDelta;
	

	//if (m_fTimeAcc >= m_fLookTime && true == m_bOneTick)
	//{
	//	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_eCurrentLevelID));
	//
	//	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//	vPlayerPos.m128_f32[1] += 1.f;
	//
	//	m_pTransformCom->Look_At(vPlayerPos);
	//
	//
	//	m_fTimeAcc = 0.f;
	//	m_bOneTick = false;
	//}
	
	
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//vPos.m128_f32[1] += m_fGravitionalConstant * fTimeDelta;
	//
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

void CBullet::Dead_Range(_float fRange)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_float fDistance = XMVectorGetX(XMVector3Length(vMyPos - XMLoadFloat4(&m_vPrevPos)));
	
	if (fDistance > fRange)
	{
		m_fLifeTime = 0.f;
		Die(m_fLifeTime);
	}

}

void CBullet::Initialize_Pos(_fvector vIntializePos)
{
	
	XMStoreFloat4(&m_vPrevPos, vIntializePos);

	
	_matrix WorldMatrix = XMLoadFloat4x4(&m_OwnerWorldMatrix);
	
	m_vPrevPos.w = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPrevPos));
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_eCurrentLevelID));

	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	vPlayerPos.m128_f32[1] += 1.f;
	m_pTransformCom->Look_At(vPlayerPos);
}

void CBullet::Init_Status(_float fMaxHp, _float fDmg)
{
	m_tStatus.fMaxHp = fMaxHp;
	m_tStatus.fDmg = fDmg;

	m_tOriginStatus = m_tStatus;
}


void CBullet::Free()
{
	__super::Free();

	if(nullptr != m_pColliderCom)
		Safe_Release(m_pColliderCom);

}
