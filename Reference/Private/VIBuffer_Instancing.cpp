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

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance; //! 인스턴스 버퍼 의 총 크기는 인스턴스 버퍼 사이즈 * 인스턴스 개수
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; //! 상태행렬은 계속 값이 변경되어야하기에 동적버퍼로 셋팅해준다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! 버텍스 버퍼다
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //! CPU에서 쓰기 권한을 허용해서 인스턴스 버퍼가 가진 상태행렬의 값을 변경할 수 있도록 하자
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++) //! 인스턴스 개수만큼 상태행렬 만들어주자
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

	ID3D11Buffer* pVertexBuffers[] = { //! 정점 버퍼와 인스턴스 버퍼. 두개의 버텍스버퍼를 바인딩하는것을 볼 수 있다.
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


	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	//! 드로우인덱스인스턴스 함수를 사용하는 것을 볼 수 있다.
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0); //! 인자값은 인덱스개수 몇개까지 한개로 칠 것인지, 인스턴스 개수는 몇개인지,  버텍스, 인덱스, 인스턴스 각각 시작오프셋은 몇인지

	return S_OK;
}



void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
