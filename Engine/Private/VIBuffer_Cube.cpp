#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1; //! 버퍼 개수
	m_iNumVertices = 8; //! 큐브의 정점 개수는 8개니까.
	m_iStride = sizeof(VTXCUBE); //! 정점 크기

	m_iNumIndices = 36; //! 인덱스 개수야.   큐브의 면은 6개. 면당 삼각형은 2개 삼각형당 인덱스는 3개 6 * 2 * 3 = 36
	m_iIndexStride = 2; //! 정점의 개수가 65535개를 넘지 앟않으면 2 넘는다면 4
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //! 삼각형으로 그릴거야

//TODO VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices; //! 정점 크기 * 정점 개수 만큼 버퍼 사이즈 잡아주자.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; //! 정적 버퍼 사용하겠다
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! 버텍스 버퍼 용으로 만드는 거다.
	m_BufferDesc.CPUAccessFlags = 0; //! 동적버퍼일 경우에만 셋팅하기에 0으로 주자.
	m_BufferDesc.MiscFlags = 0; //! 위와 같다
	m_BufferDesc.StructureByteStride = m_iStride; //! 구조체 하나의 크기는 정점 구조체크기다.

	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	
	VTXCUBE*	pVertices = new VTXCUBE[m_iNumVertices]; //! 정점 개수만큼 할당해서 값 채우자.
	
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition; //! 정점위치가 곧 방향벡터이다.

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_SubResourceData.pSysMem = pVertices; //! m_SubResourceData의 이제 이 정점들의 정보를 넣어주자.

	if(FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices); //! 서브리소스 데이터에 정보 채워서 버텍스 버퍼 도 만들어줬으니 할당했던 정점들은 지워주자.

//TODO INDEX_BUFFER



	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
