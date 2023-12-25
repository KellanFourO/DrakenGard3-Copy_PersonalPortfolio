#include "BoundingBox_OBB.h"
#include "DebugDraw/DebugDraw.h"
#include "Collider.h"

CBoundingBox_OBB::CBoundingBox_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_OBB::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_OBB_DESC* pDesc = (BOUNDING_OBB_DESC*)pBoundingDesc;

	_float4		vQuaternion = {};
	
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginBoundingOriented = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);

	return S_OK;
}

void CBoundingBox_OBB::Update(_fmatrix TransformMatrix)
{
	m_pOriginBoundingOriented->Transform(*m_pBoundingOriented, TransformMatrix);
}

_bool CBoundingBox_OBB::Collision(CCollider* pTargetCollider, _bool* pisCollision)
{
	CBoundParent* pTargetBounding = pTargetCollider->Get_Bounding();

	*pisCollision = false;

	switch (pTargetCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		*pisCollision = m_pBoundingOriented->Intersects(*((CBoundingBox_AABB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		*pisCollision = m_pBoundingOriented->Intersects(*((CBoundingBox_OBB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_Sphere:
		*pisCollision = m_pBoundingOriented->Intersects(*((CBoundingBox_Sphere*)pTargetBounding)->Get_Bounding());
		break;
	}
	return *pisCollision;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor)
{
	if (nullptr == pBatch ||
		nullptr == m_pBoundingOriented)
		return E_FAIL;

	/* m_pAABB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pBoundingOriented);

	return S_OK;
}
#endif

CBoundingBox_OBB * CBoundingBox_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BOUNDING_DESC* pBoundingDesc)
{
	CBoundingBox_OBB*		pInstance = new CBoundingBox_OBB(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Created : CBoundingBox_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingBox_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginBoundingOriented);
	Safe_Delete(m_pBoundingOriented);
}
