#include "Cell.h"

#ifdef _DEBUG
	#include "Shader.h"
	#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)

{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

	//!직선의 수직벡터를 구하는 법은 쉽다. 직선의 x,z를 스왑시킨후 x를 음수로 두면된다.
	//! 
	_vector vLine = XMLoadFloat3(&pPoints[POINT_B]) - XMLoadFloat3(&pPoints[POINT_A]);
	XMStoreFloat3(&m_vNormals[LINE_AB], XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
	
	vLine = XMLoadFloat3(&pPoints[POINT_C]) - XMLoadFloat3(&pPoints[POINT_B]);
	XMStoreFloat3(&m_vNormals[LINE_BC], XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));

	vLine = XMLoadFloat3(&pPoints[POINT_A]) - XMLoadFloat3(&pPoints[POINT_C]);
	XMStoreFloat3(&m_vNormals[LINE_CA], XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));

#ifdef _DEBUG

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice,m_pContext, pPoints);

	if(nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
{
	//TODO XMVectorEqual과 XMVector3Equal

	//!XMVectorEqual은 결과값으로 새로운 벡터를 리턴한다.
	//! x는 x, y는 y처럼 x,y,z,w를 각각 비교해서 만약 x,z가 같다면 { 1, 0 , 1, 0, } true,false를 가지고있는 벡터를 리턴하는 것

	//!XMVector3Eqaul은 결과값으로 x,y,z,w가 모두 같다면 true 아니면 false를 리턴하는 형태

	//!Navigation이 가지고있는 셀들을 이중 순회한다. #셀이중순회

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pSourPoint)))
	{
		if(true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}
	
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pSourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(pSourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex)
{
	//TODO 왜 월드행렬을 곱하는거야?

	//! 이전에는 월드상제 존재한다고 가정하고 있었었어.
	//! 하지만 만약 지형이 움직인다고 가정해보자고, 그러면 네비게이션이 렌더링만 월드위치로 그릴게아니라 실제 위치도 월드로 갈 필요가있었어.
	//! 전체 셀이 월드위치로 갈 필요는 없었고. 내가 비교할 셀만 월드위치로 올리는게 훨씬 값이 싸게 먹혀.
	//! 그래서 클라이언트 지형객체가 네비게이션으로 월드행렬을 던지고 네비게이션이 셀에게 월드행렬을 던져주는 과정이 있던거고.
	//! 그 지형객체의 월드행렬을 곱해주는 형태로 가져간거지. 마찬가지로 그릴때도 월드행렬 던져줘야겟지

	for (size_t i = 0; i < LINE_END; ++i)
	{
		_vector vStartPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), WorldMatrix);
		_vector vNormal = XMVector3TransformNormal(XMLoadFloat3(&m_vNormals[i]), WorldMatrix);

		_vector vSourDir = vPosition - vStartPoint;

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSourDir),
			XMVector3Normalize(vNormal))))
		{
			*pNeighborIndex = m_iNeighbors[i];
			return false;
		}
	}

	return true;
}

//void CCell::Compute_Height(_float3& vPosition, _float& fY)
//{
//	_vector vPlane = {};
//
//	_vector vPointA = XMLoadFloat3(&m_vPoints[POINT_A]);
//
//	_vector vPointB =
//	_vector vPointC =
//		, XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&m_vPoints[POINT_C]
//
//	vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&m_vPoints[POINT_C]));
//
//	_float4 vResultPlane = {};
//
//	XMStoreFloat4(&vResultPlane, vPlane);
//
//	if (vResultPlane.y > 0.f)
//	{
//		fY = (-resultPlane.x * vPosition.x - resultPlane.z * vPosition.z - resultPlane.w) / resultPlane.y;
//	}
//}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_VIBuffers();

	m_pVIBuffer->Render();
	
	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
