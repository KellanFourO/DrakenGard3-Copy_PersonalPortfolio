
#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"
#include "Cell.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_bSleep(rhs.m_bSleep)
	, m_bUseGravity(rhs.m_bUseGravity)
	, m_bKinematic(rhs.m_bKinematic)
	, m_fFriction(rhs.m_fFriction)
	, m_fMass(rhs.m_fMass)
	, m_fGravitionalConstant(rhs.m_fGravitionalConstant)
	, m_byConstraints(rhs.m_byConstraints)
	, m_vLinearAcceleration(rhs.m_vLinearAcceleration)
	, m_vLinearVelocity(rhs.m_vLinearVelocity)
	, m_fSleepThreshold(rhs.m_fSleepThreshold)
{

}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memmove(&m_eType, pArg, sizeof(RIGIDBODY_TYPE));

	return S_OK;
}

void CRigidBody::Tick(_float fTimeDelta)
{
	if (Check_Sleep() || m_eType == RIGIDBODY_TYPE::STATIC)
		return;

	m_bKinematic ? Update_Kinematic(fTimeDelta) : Update_Kinetic(fTimeDelta);
}

void CRigidBody::Add_Force(const _float3& vForce, const FORCE_MODE& eMode)
{
	_vector vVector = XMLoadFloat3(&vForce);

	

	switch (eMode)
	{
	case FORCE_MODE::FORCE:
		XMStoreFloat3(&m_vLinearAcceleration, (XMLoadFloat3(&m_vLinearAcceleration) += vVector / m_fMass));
		break;
	case FORCE_MODE::IMPULSE:
		XMStoreFloat3(&m_vLinearVelocity, (XMLoadFloat3(&m_vLinearVelocity) += vVector / m_fMass));
		break;
	case FORCE_MODE::ACCELERATION:
		XMStoreFloat3(&m_vLinearAcceleration, (XMLoadFloat3(&m_vLinearAcceleration) += vVector));
		break;
	case FORCE_MODE::VELOCITY_CHANGE:
		XMStoreFloat3(&m_vLinearVelocity, (XMLoadFloat3(&m_vLinearVelocity) += vVector));
		break;
	default:
		break;
	}

	Wake();
}

void CRigidBody::Clear_Force(const FORCE_MODE& eMode)
{
	switch (eMode)
	{
	case FORCE_MODE::FORCE:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(_float3));
		break;
	case FORCE_MODE::IMPULSE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(_float3));
		break;
	case FORCE_MODE::ACCELERATION:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(_float3));
		break;
	case FORCE_MODE::VELOCITY_CHANGE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(_float3));
		break;
	default:
		break;
	}
}

void CRigidBody::Clear_NetPower()
{
	ZeroMemory(&m_vLinearAcceleration, sizeof(_float3));
	ZeroMemory(&m_vLinearVelocity, sizeof(_float3));
}

void CRigidBody::Update_Kinetic(const _float& fTimeDelta)
{
	_vector vPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_float3 vRealPos;
	XMStoreFloat3(&vRealPos, vPos);
	
	_float fY = m_pOwnerNavigation->Compute_Height(vRealPos);

	if (m_bUseGravity && vPos.m128_f32[1] > fY)
		m_vLinearVelocity.y += m_fGravitionalConstant * fTimeDelta;

	XMStoreFloat3(&m_vLinearVelocity, XMLoadFloat3(&m_vLinearVelocity) + (XMLoadFloat3(&m_vLinearAcceleration) * fTimeDelta));
//	m_vLinearVelocity += m_vLinearAcceleration * fTimeDelta;

	const _float fLinearResistance = m_fFriction;

	XMStoreFloat3(&m_vLinearVelocity, (fLinearResistance < 1.f) ? (XMLoadFloat3(&m_vLinearVelocity) * (1.f - fLinearResistance)) : (XMLoadFloat3(&m_vLinearVelocity) = XMVectorZero()));

	if (m_byConstraints)
	{
		_int i = 0;
		while (i < 3)
			(m_byConstraints & 1 << i) ? (*(((_float*)&m_vLinearVelocity) + i++) = 0) : i++;
	}

	Update_Transform(fTimeDelta);
}

void CRigidBody::Update_Kinematic(const _float& fTimeDelta)
{
	Clear_Force(FORCE_MODE::VELOCITY_CHANGE);
}

void CRigidBody::Update_Transform(const _float& fTimeDelta)
{
	_vector vPos = XMLoadFloat3(&m_vLinearVelocity) * fTimeDelta;
	
	_float3 vRealPos;
	XMStoreFloat3(&vRealPos, vPos);

	m_pOwner->Get_Transform()->Translate(vRealPos, m_pOwnerNavigation);
}

const _bool CRigidBody::Check_Sleep()
{
	if (m_bSleep)
		return TRUE;

	if (!m_bUseGravity && m_fSleepThreshold > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vLinearVelocity))))
	{
		Sleep();
		return TRUE;
	}

	return FALSE;
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

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
}
