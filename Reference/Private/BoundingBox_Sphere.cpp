#include "..\Public\BoundingBox_Sphere.h"


CBoundingBox_Sphere::CBoundingBox_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_Sphere::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = (BOUNDING_SPHERE_DESC*)pBoundingDesc;

	m_pBoundingSphere = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);	

	return S_OK;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	if (nullptr == pBatch ||
		nullptr == m_pBoundingSphere)
		return E_FAIL;

	/* m_pAABB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pBoundingSphere);

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


}
