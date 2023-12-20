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
	m_iNumVertexBuffers = 1; //! ���� ����
	m_iNumVertices = 8; //! ť���� ���� ������ 8���ϱ�.
	m_iStride = sizeof(VTXCUBE); //! ���� ũ��

	m_iNumIndices = 36; //! �ε��� ������.   ť���� ���� 6��. ��� �ﰢ���� 2�� �ﰢ���� �ε����� 3�� 6 * 2 * 3 = 36
	m_iIndexStride = 2; //! ������ ������ 65535���� ���� �۾����� 2 �Ѵ´ٸ� 4
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //! �ﰢ������ �׸��ž�

//TODO VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices; //! ���� ũ�� * ���� ���� ��ŭ ���� ������ �������.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; //! ���� ���� ����ϰڴ�
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! ���ؽ� ���� ������ ����� �Ŵ�.
	m_BufferDesc.CPUAccessFlags = 0; //! ���������� ��쿡�� �����ϱ⿡ 0���� ����.
	m_BufferDesc.MiscFlags = 0; //! ���� ����
	m_BufferDesc.StructureByteStride = m_iStride; //! ����ü �ϳ��� ũ��� ���� ����üũ���.

	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	
	VTXCUBE*	pVertices = new VTXCUBE[m_iNumVertices]; //! ���� ������ŭ �Ҵ��ؼ� �� ä����.
	
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition; //! ������ġ�� �� ���⺤���̴�.

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

	m_SubResourceData.pSysMem = pVertices; //! m_SubResourceData�� ���� �� �������� ������ �־�����.

	if(FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices); //! ���긮�ҽ� �����Ϳ� ���� ä���� ���ؽ� ���� �� ����������� �Ҵ��ߴ� �������� ��������.

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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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
