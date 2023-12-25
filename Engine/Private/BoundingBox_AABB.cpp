#include "BoundingBox_AABB.h"
#include "DebugDraw/DebugDraw.h"
#include "Collider.h"

CBoundingBox_AABB::CBoundingBox_AABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_AABB::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_AABB_DESC* pDesc = (BOUNDING_AABB_DESC*)pBoundingDesc;

	m_pOriginBoundingBox = new BoundingBox(pDesc->vCenter, pDesc->vExtents);

	return S_OK;
}

void CBoundingBox_AABB::Update(_fmatrix TransformMatrix)
{
	m_pOriginBoundingBox->Transform(*m_pBoundingBox, TransformMatrix);
}

_bool CBoundingBox_AABB::Collision(CCollider* pTargetCollider, _bool* pisCollision)
{
	CBoundParent* pTargetBounding = pTargetCollider->Get_Bounding();

	*pisCollision = false;

	switch (pTargetCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		*pisCollision = m_pBoundingBox->Intersects(*((CBoundingBox_AABB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		*pisCollision = m_pBoundingBox->Intersects(*((CBoundingBox_OBB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_Sphere:
		*pisCollision = m_pBoundingBox->Intersects(*((CBoundingBox_Sphere*)pTargetBounding)->Get_Bounding());
		break;
	}
	return *pisCollision;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor)
{
	if (nullptr == pBatch ||
		nullptr == m_pBoundingBox)
		return E_FAIL;

	/* m_pAABB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pBoundingBox);

	return S_OK;
}
#endif

CBoundingBox_AABB * CBoundingBox_AABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BOUNDING_DESC* pBoundingDesc)
{
	CBoundingBox_AABB*		pInstance = new CBoundingBox_AABB(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Created : CBoundingBox_AABB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingBox_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginBoundingBox);
	Safe_Delete(m_pBoundingBox);

}
