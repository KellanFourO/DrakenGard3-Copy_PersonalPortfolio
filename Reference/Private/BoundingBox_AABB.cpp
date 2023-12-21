#include "BoundingBox_AABB.h"

CBoundingBox_AABB::CBoundingBox_AABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_AABB::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_AABB_DESC* pDesc = (BOUNDING_AABB_DESC*)pBoundingDesc;

	m_pBoundingBox = new BoundingBox(pDesc->vCenter, pDesc->vExtents);	

	return S_OK;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
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


}
