#include "BoundingBox_OBB.h"


CBoundingBox_OBB::CBoundingBox_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBoundParent(pDevice, pContext)
{

}

HRESULT CBoundingBox_OBB::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_OBB_DESC* pDesc = (BOUNDING_OBB_DESC*)pBoundingDesc;

	_float4		vQuaternion = {};
	
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pBoundingOriented = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);

	return S_OK;
}

#ifdef _DEBUG	
HRESULT CBoundingBox_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
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


}
