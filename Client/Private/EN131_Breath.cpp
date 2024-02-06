#include "stdafx.h"
#include "EN131_Breath.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Player.h"
#include "Effect_BossBreath.h"
#include "Effect_BornFire.h"
#include "Particle_Object.h"

CEN131_Breath::CEN131_Breath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBullet(pDevice, pContext)
{
}

CEN131_Breath::CEN131_Breath(const CEN131_Breath& rhs)
    : CBullet(rhs)
{
}

HRESULT CEN131_Breath::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

    return S_OK;
}

HRESULT CEN131_Breath::Initialize(void* pArg)
{
	CEN131_Breath::EN131_BREATHDESC Desc = {};
	Desc = *(EN131_BREATHDESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	_float4 vPos = { m_OwnerWorldMatrix.m[3][0], m_OwnerWorldMatrix.m[3][1], m_OwnerWorldMatrix.m[3][2], 1.f };
	_vector vRealPos = XMLoadFloat4(&vPos);

	XMStoreFloat4(&m_vPrevPos, vRealPos);
	m_vPrevPos.w = 1.f;
	_float3 vOwnerLook; 
	vOwnerLook = { Desc.vLook.x, Desc.vLook.y , Desc.vLook.z };
	
	m_vParentLook = Desc.vParentLook;
	//vOwnerLook.y *= 0.5f;

	switch (Desc.eBreathType)
	{
		case CEN131_Breath::BREATH_LEFT:
			vOwnerLook = m_pTransformCom->Get_RotateDir(vOwnerLook, -15.f);
			break;
		
		case  CEN131_Breath::BREATH_RIGHT:
			vOwnerLook = m_pTransformCom->Get_RotateDir(vOwnerLook, 15.f);
			break;
	}

	m_eBreathType = Desc.eBreathType;

	m_pTransformCom->Look_At_Dir(vOwnerLook);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPrevPos));
	//m_pTransformCom->Look_At(XMLoadFloat4(&Desc.vTargetPos));
	
	Init_Status(0.f, 50.f);
	m_tStatus.eAttackType = tagStatusDesc::NORMAL_ATTACK;
	m_pColliderCom->OnAccCollider(1.f);
	
	//__super::Initialize_Pos(vRealPos);
	
	m_fLifeTime = 5.f;

    return S_OK;
}

void CEN131_Breath::Priority_Tick(_float fTimeDelta)
{
}

void CEN131_Breath::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	
	if (m_fTimeAcc >= m_fLifeTime)
	{
		m_fLifeTime = 0.2f;
		No_DissoveDie(m_fLifeTime);
		
	}

	if (m_bCreateEffect == false)
	{
		Create_Effect();
		m_bCreateEffect = true;
		m_fEffectCreateTime = 0.25f;
	}
	
		m_fEffectTimeAcc += fTimeDelta;

	if (m_bCreateEffect == true && m_fEffectTimeAcc >= m_fEffectCreateTime)
	{
	
		Create_Effect();
		m_fEffectTimeAcc = 0.f;
	}

	//if (m_bCreateEffect == false)
	//{
	//	Create_Effect();
	//	m_bCreateEffect = true;
	//}

		//m_bCreateEffect = true;
	//}

}

void CEN131_Breath::Late_Tick(_float fTimeDelta)
{
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CEN131_Breath::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	
		m_pShaderCom->Begin(0);

#ifdef _DEBUG

	m_pColliderCom->Render();
#endif // _DEBUG
    return S_OK;
}

void CEN131_Breath::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	__super::On_Collision(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);
}

void CEN131_Breath::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag, bType, bHit);
}

void CEN131_Breath::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);
}

void CEN131_Breath::Initialize_Pos(_fvector vIntializePos)
{
	XMStoreFloat4(&m_vPrevPos, vIntializePos);


	_matrix WorldMatrix = XMLoadFloat4x4(&m_OwnerWorldMatrix);

	m_vPrevPos.w = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPrevPos));

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_eCurrentLevelID));

	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	vPlayerPos.m128_f32[1] += 5.f;
	m_pTransformCom->Look_At(vPlayerPos);
}

HRESULT CEN131_Breath::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_OBB::BOUNDING_OBB_DESC BoundingDesc = {};

	BoundingDesc.vExtents = _float3(2.4f, 1.f, 10.f);
	BoundingDesc.vCenter = _float3(0.f, -2.5f, 13.f);
	BoundingDesc.vRotation = _float3(XMConvertToRadians(0.f), 0.f, 0.f);
	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_WEAPON;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;




	m_pColliderCom->OnCollider();

	m_vecColliders.push_back(m_pColliderCom);
	
	m_bHitDead = false;
	
    return S_OK;
}

HRESULT CEN131_Breath::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

    return S_OK;
}

void CEN131_Breath::Create_Effect()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	// 방향 벡터를 기준으로 하는 각도 계산 (라디안)

	float radianAngle = atan2(vMyLook.m128_f32[1], vMyLook.m128_f32[0]);

	vMyPos.m128_f32[0] = vMyPos.m128_f32[0] + vMyLook.m128_f32[0] * 4.f;
	vMyPos.m128_f32[1] = vMyPos.m128_f32[1] - 10.f;

 	CParticle_Object::PARTICLE_DESC PaticleDesc;
 
 	PaticleDesc.bRandom = true;
 	PaticleDesc.iNumInstance = 800;
 	PaticleDesc.iShaderPathIndex = 2;
 	PaticleDesc.fRange = 15.f;
	PaticleDesc.vCenter = { vMyPos.m128_f32[0], vMyPos.m128_f32[1], vMyPos.m128_f32[2] };
 	PaticleDesc.vSpeed = { 5.f, 5.f };
 	PaticleDesc.vScale = { 1.f, 10.f };
 	//PaticleDesc.vRotation = { m_vParticleRotation[0], m_vParticleRotation[1], m_vParticleRotation[2] };
 	//PaticleDesc.vRandomRotation = { radianAngle - XMConvertToRadians(45.f), radianAngle }; //! 
	PaticleDesc.vRandomRotation = { 160.f, 180.f }; //! 
 	//PaticleDesc.vInterval = { m_vParticleInterval[0], m_vParticleInterval[1], m_vParticleInterval[2] };
 	PaticleDesc.vDir = { vMyLook.m128_f32[0], vMyLook.m128_f32[1], vMyLook.m128_f32[2], 0.f };
// 	PaticleDesc.vColor = { m_vParticleColor[0], m_vParticleColor[1], m_vParticleColor[2], m_vParticleColor[3] };
 	PaticleDesc.vLifeTime = { 3.f, 3.f };
// 
 	PaticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_BornFire");
	PaticleDesc.bTimeScale = true;
		

		
		//_float4 vRandomPos = m_pTransformCom->Get_RandomPositionAroundCenter(vMyPos, XMConvertToRadians(180.f));

		//CEffect_BornFire::BORNFIRE_DESC Desc;
		//
		//Desc.fSpeedPerSec = 8.f;
		//Desc.fRotationPerSec = XMConvertToRadians(90.f);
		//Desc.pTarget = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY);
		//Desc.vPos;
		//Desc.bBossBreath = true;
		//Desc.vScale = _float3( 8.f, 5.f, 5.f);
		//XMStoreFloat4(&Desc.vPos, vMyPos);
		//
		//Desc.fLifeTime = 3.f;
		//XMStoreFloat4(&Desc.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		
		
		//Desc.vLook = m_vParentLook;

		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Object"), &PaticleDesc);
	
}

CEN131_Breath* CEN131_Breath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEN131_Breath* pInstance = new CEN131_Breath(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEN131_Breath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEN131_Breath::Clone(void* pArg)
{
	CEN131_Breath* pInstance = new CEN131_Breath(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEN131_Breath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEN131_Breath::Free()
{
	__super::Free();

	
}
