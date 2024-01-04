#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	INSTANCING_DESC		InstancingDesc = *(INSTANCING_DESC*)pArg;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance; //! �ν��Ͻ� ���� �� �� ũ��� �ν��Ͻ� ���� ������ * �ν��Ͻ� ����
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; //! ��������� ��� ���� ����Ǿ���ϱ⿡ �������۷� �������ش�.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! ���ؽ� ���۴�
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //! CPU���� ���� ������ ����ؼ� �ν��Ͻ� ���۰� ���� ��������� ���� ������ �� �ֵ��� ����
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++) //! �ν��Ͻ� ������ŭ ������� ���������
	{
		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertices[i].vPosition = _float4(rand() % 10, rand() % 10, rand() % 10, 1.f);
	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB ||
		nullptr == m_pVBInstance)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { //! ���� ���ۿ� �ν��Ͻ� ����. �ΰ��� ���ؽ����۸� ���ε��ϴ°��� �� �� �ִ�.
		m_pVB,
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};


	/* � ���ؽ� ���۵��� �̿��ҰŴ�. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* � �ε��� ���۸� �̿��ҰŴ�. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* ������ ������� �̾ �׸��Ŵ�. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	//! ��ο��ε����ν��Ͻ� �Լ��� ����ϴ� ���� �� �� �ִ�.
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0); //! ���ڰ��� �ε������� ����� �Ѱ��� ĥ ������, �ν��Ͻ� ������ �����,  ���ؽ�, �ε���, �ν��Ͻ� ���� ���ۿ������� ������

	return S_OK;
}



void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
