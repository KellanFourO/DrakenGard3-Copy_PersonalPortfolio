#include "..\Public\BoundingBox_Sphere.h"
#include "DebugDraw/DebugDraw.h"
#include "Collider.h"

CBoundingBox_Sphere::CBoundingBox_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_Sphere::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = (BOUNDING_SPHERE_DESC*)pBoundingDesc;

	

	m_pOriginBoundingSphere = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);	
	m_pBoundingSphere = new BoundingSphere(*m_pOriginBoundingSphere);

	return S_OK;
}

void CBoundingBox_Sphere::Update(_fmatrix TransformMatrix)
{
	m_pOriginBoundingSphere->Transform(*m_pBoundingSphere, TransformMatrix);
}

_bool CBoundingBox_Sphere::Collision(CCollider* pTargetCollider, _bool* pisCollision)
{
	CBoundParent* pTargetBounding = pTargetCollider->Get_Bounding();

	*pisCollision = false;

	switch (pTargetCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		*pisCollision = m_pBoundingSphere->Intersects(*((CBoundingBox_AABB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		*pisCollision = m_pBoundingSphere->Intersects(*((CBoundingBox_OBB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_Sphere:
		*pisCollision = m_pBoundingSphere->Intersects(*((CBoundingBox_Sphere*)pTargetBounding)->Get_Bounding());
		break;
	}
	return *pisCollision;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor)
{
	if (nullptr == pBatch ||
		nullptr == m_pBoundingSphere)
		return E_FAIL;

	/* m_pAABB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pBoundingSphere, vColor);

	return S_OK;
}
#endif

CBoundingBox_Sphere * CBoundingBox_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BOUNDING_DESC* pBoundingDesc)
{
	CBoundingBox_Sphere*		pInstance = new CBoundingBox_Sphere(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Created : CBoundingBox_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingBox_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginBoundingSphere);
	Safe_Delete(m_pBoundingSphere);
}
