#include "RigidBody.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
	, m_fGravityAcc(rhs.m_fGravityAcc)
	, m_fHeavy(rhs.m_fHeavy)
	, m_pOwnerTransformCom(rhs.m_pOwnerTransformCom)
	, m_vNetPower(rhs.m_vNetPower)
{
	Safe_AddRef(m_pOwnerTransformCom);
}

HRESULT CRigidBody::Initialize_Prototype()
{
	ZeroMemory(&m_vNetPower, sizeof(_float3));

	
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{

	m_pOwnerTransformCom = static_cast<CTransform*>(pArg);

	AddRefIfNotNull(m_pOwnerTransformCom);

	return S_OK;
}

void CRigidBody::Priority_Tick(_float fTimeDelta)
{
}

void CRigidBody::Tick(_float fTimeDelta)
{
	m_fGravityAcc = m_fHeavy * -9.8f * 1.3f;
	if (m_isKinematic)
	{
		//Get_Transform()->Translate(fTimeDelta * m_vNetPower);
		if (nullptr != m_pOwnerTransformCom)
		{
			m_pOwnerTransformCom->Translate(fTimeDelta * XMLoadFloat3(&m_vNetPower));
		}
	}
	if (m_UseGravity)
	{
		m_vNetPower.y += fTimeDelta * m_fGravityAcc;
		// 임시 중력 해제
		/*if (m_pTransformCom->m_vInfo[INFO_POS].y >= m_pTransformCom->LocalScale().y + 2.1f)
			m_vNetPower.y += fTimeDelta * m_fGravityAcc;*/
		if (m_vNetPower.y < m_fGravityAcc)
			m_vNetPower.y = m_fGravityAcc;
	}
	/*else
	{
		Get_Transform()->Translate(fTimeDelta * m_vNetPower);

	}*/

	if (nullptr != m_pOwnerTransformCom)
	{

		/*_vector Get_State(STATE eState)
		{
			return XMVectorSet
			(
				m_WorldMatrix.m[eState][0],
				m_WorldMatrix.m[eState][1],
				m_WorldMatrix.m[eState][2],
				m_WorldMatrix.m[eState][3]
			);
		}*/
		_float3 vPosition;
		XMStoreFloat3(&vPosition, m_pOwnerTransformCom->Get_State(CTransform::STATE_POSITION));
		
		if (vPosition.y < 1.f)
		{
			vPosition.y = 1.f;

			m_pOwnerTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));
		
		}
	}


	//if (Get_Transform()->m_vInfo[INFO_POS].y < 1.f)
	//	Get_Transform()->m_vInfo[INFO_POS].y = 1.f;

	
}

void CRigidBody::Late_Tick(_float fTimeDelta)
{
}

void CRigidBody::Add_Force(_float3 _vForce)
{
	_vector vAddForce = XMLoadFloat3(&m_vNetPower) + XMLoadFloat3(&_vForce);

	XMStoreFloat3(&m_vNetPower, vAddForce);
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerTransformCom);
}
