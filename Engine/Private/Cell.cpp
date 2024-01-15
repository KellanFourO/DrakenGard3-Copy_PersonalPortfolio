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

	//!������ �������͸� ���ϴ� ���� ����. ������ x,z�� ���ҽ�Ų�� x�� ������ �θ�ȴ�.
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
	//TODO XMVectorEqual�� XMVector3Equal

	//!XMVectorEqual�� ��������� ���ο� ���͸� �����Ѵ�.
	//! x�� x, y�� yó�� x,y,z,w�� ���� ���ؼ� ���� x,z�� ���ٸ� { 1, 0 , 1, 0, } true,false�� �������ִ� ���͸� �����ϴ� ��

	//!XMVector3Eqaul�� ��������� x,y,z,w�� ��� ���ٸ� true �ƴϸ� false�� �����ϴ� ����

	//!Navigation�� �������ִ� ������ ���� ��ȸ�Ѵ�. #�����߼�ȸ

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
	//TODO �� ��������� ���ϴ°ž�?

	//! �������� ������� �����Ѵٰ� �����ϰ� �־�����.
	//! ������ ���� ������ �����δٰ� �����غ��ڰ�, �׷��� �׺���̼��� �������� ������ġ�� �׸��Ծƴ϶� ���� ��ġ�� ����� �� �ʿ䰡�־���.
	//! ��ü ���� ������ġ�� �� �ʿ�� ������. ���� ���� ���� ������ġ�� �ø��°� �ξ� ���� �ΰ� ����.
	//! �׷��� Ŭ���̾�Ʈ ������ü�� �׺���̼����� ��������� ������ �׺���̼��� ������ ��������� �����ִ� ������ �ִ��Ű�.
	//! �� ������ü�� ��������� �����ִ� ���·� ����������. ���������� �׸����� ������� ������߰���

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

    /* ������ü�� �ʱ�ȭ�Ѵ�.  */
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
