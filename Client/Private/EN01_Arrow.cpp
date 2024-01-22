#include "stdafx.h"
#include "EN01_Arrow.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Player.h"

CEN01_Arrow::CEN01_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBullet(pDevice, pContext)
{
}

CEN01_Arrow::CEN01_Arrow(const CEN01_Arrow& rhs)
    : CBullet(rhs)
{
}

HRESULT CEN01_Arrow::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

    return S_OK;
}

HRESULT CEN01_Arrow::Initialize(void* pArg)
{
	CEN01_Arrow::EN01_ARROWDESC Desc = {};

	if (nullptr != pArg)
	{
		Desc = *(EN01_ARROWDESC*)pArg;
		m_eArrowType = Desc.eArrowType;
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	_float4 vPos = { m_OwnerWorldMatrix.m[3][0], m_OwnerWorldMatrix.m[3][1], m_OwnerWorldMatrix.m[3][2], 1.f };
	_vector vRealPos = XMLoadFloat4(&vPos);

	if(m_eArrowType == CEN01_Arrow::ARROW_NORMAL || m_eArrowType == CEN01_Arrow::ARROW_C_NORMAL)
		__super::Initialize_Pos(vRealPos);
	else
		Initialize_Pos(vRealPos);
	

    return S_OK;
}

void CEN01_Arrow::Priority_Tick(_float fTimeDelta)
{
}

void CEN01_Arrow::Tick(_float fTimeDelta)
{
	switch (m_eArrowType)
	{
		case Client::CEN01_Arrow::ARROW_NORMAL:
		{
			__super::Go_Straight(fTimeDelta);
			break;
		}
		case Client::CEN01_Arrow::ARROW_PARABOLIC:
		{
			__super::Go_Parabolic(fTimeDelta);
			break;
		}
		case Client::CEN01_Arrow::ARROW_C_NORMAL:
		{
			__super::Go_Straight(fTimeDelta);
			break;
		}
		case Client::CEN01_Arrow::ARROW_C_PARABOLIC:
		{
			__super::Go_Parabolic(fTimeDelta);
			break;
		}
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	__super::Dead_Range(m_fRange);
}

void CEN01_Arrow::Late_Tick(_float fTimeDelta)
{
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CEN01_Arrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);


		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	m_pColliderCom->Render();
#endif // _DEBUG
    return S_OK;
}

void CEN01_Arrow::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	__super::On_Collision(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);
}

void CEN01_Arrow::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag, bType, bHit);
}

void CEN01_Arrow::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);
}

void CEN01_Arrow::Initialize_Pos(_fvector vIntializePos)
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

HRESULT CEN01_Arrow::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Monster_EN01_Weapon_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.fRadius = 0.5f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, -0.6f);
	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_WEAPON;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->OnCollider();

	m_vecColliders.push_back(m_pColliderCom);
	
    return S_OK;
}

HRESULT CEN01_Arrow::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

    return S_OK;
}

CEN01_Arrow* CEN01_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEN01_Arrow* pInstance = new CEN01_Arrow(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEN01_Arrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEN01_Arrow::Clone(void* pArg)
{
	CEN01_Arrow* pInstance = new CEN01_Arrow(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEN01_Arrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEN01_Arrow::Free()
{
	__super::Free();

	
}
