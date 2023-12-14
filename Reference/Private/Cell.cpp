#include "Cell.h"

#ifdef _DEBUG
	#include "Shader.h"
	#include "VIBuffer_Cell.h"
	#include "GameInstance.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
#ifdef _DEBUG
	, m_pGameInstance(CGameInstance::GetInstance())
#endif
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

#ifdef _DEBUG
	Safe_AddRef(m_pGameInstance);
#endif
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3));

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

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; ++i)
	{
		_vector vSourDir = vPosition - XMLoadFloat3(&m_vPoints[i]);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSourDir),
			XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])))))
		{
			*pNeighborIndex = m_iNeighbors[i];
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(class CShader* pShader)
{
	_float4x4 WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if(FAILED(pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	pShader->Begin(0);

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
	Safe_Release(m_pGameInstance);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
