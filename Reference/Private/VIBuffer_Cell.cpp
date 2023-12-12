#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoint)
{
	m_iNumVertexBuffers = 1; //! ���߿��� �������� ���۸� ����Ҽ� �־. ����� 1���� ���
	m_iNumVertices = 3; //!  �ﰢ�� ���� ���� 3��
	m_iStride = sizeof(VTXPOS); //! ���� �Ѱ��� ũ��� �� ������ ������ �־���� ������ ������(����ü)F

	m_iNumIndices = 4; //! ���ν�Ʈ������ �׸��ž�. �ε����� 0, 1, 2, 0 �� 4����.

	//TODO �ε��� ����� ���� ����
	//! ���� �ε����� ������� 2 ����Ʈ�� 4 ����Ʈ �� �߿� �ϳ��� ����� �����ϴ�.
	//! 2 ����Ʈ�� ��쿡�� unsigned short�� ������, 4 ����Ʈ�� ��쿡�� unsigned int�� ����� ���Ѵ�.
	//! ��, ������ ������ unsigned short�� ���� �� �ִ� �ִ� ���� 65535�� �Ѵ´ٸ� ����� 4�� ����ϰ�, �ƴ϶�� 2�� �شٴ� ���
	
	m_iIndexStride = 2; //! ���⼭�� ������ ������ 65535�� ���� ���� ���̴� 2�� �� ���̴�.

	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEX_BUFFER 
//TODO ���� ������ ���� ������ �־��ִ� ����

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc); //! �ʱ�ȭ

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices; //! �Ҵ��� ũ�⸦ ���Ѵ�.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/; //! ���� ���۸� ����Ѵ�
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! �������� ���� ä�� �Ŵ�
	m_BufferDesc.CPUAccessFlags = 0; //! ���� ������ ��쿡�� ��ȿ�� ���̴�. �������۴� 0���� ä���
	m_BufferDesc.MiscFlags = 0; //! ���� ����

	//todo StructureByteStride : ����� �Ҵ�� ������ �����͸� ������ � ������ ���� ���̳�
	m_BufferDesc.StructureByteStride = m_iStride; //! ���� ũ�� ������ �����Ŵ�.

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);


	//TODO ������ ���� �̾߱�
	//! ������ �ٲ۴ٴ� ���� ���� ������ �迭���� �����ؼ� ���� �������� ������ �ٲ��� ���� ���Ѵ�.
	//! �츮�� ������ ������������ �ϴ� ������, ���� ������ �����͸� �����ؼ� �װ� ��ȯ���Ѽ� ����ϴ� ����.
	//! ��, ������ ������������ ������ �ٲٴ� ������ �ƴ϶�, ��ȯ�ؼ� ����ϴ� ����


	//TODO ���ع���
	//! �ٷ� �Ʒ����� VTXPOSTEX ����ü�� �Ҵ��ϴ� ������ m_SubResourceData.pSysMem�� ���� ä����� �ϴ� ����
	//! ���� ���۸� ������ٴ� ������ �ƴ϶�� ��.
	//! �׷��� ���� ä���ְ� 71�� ���ο��� ��¥�� ���۸� ���� �ѵ� �ʿ���������� �����ϴ� ���̴�.

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	//TODO �������� ������ 1 �������� �����ϴ� ����
	//! ���� ������ �߾��� 0,0 �̴�. 
	//! ���Ͱ��� 1�������� ��������� ���߿� ������ ���� �ٶ� ����� ������ �ʿ���� ���ϴ� ���� �� �� �ִ�
	//! ex) m_pTransform->Set_Scale(15.f,15.f,15.f), ���� 1������ �ƴ϶� 2�����̾��ٸ� �������� 30�� �Ǿ��� ��. 
	//! 1�����̶� �������� 15�� �Ǵ� ���̴�.
	//! ��, -0.5, 0, +0.5. ���� �������� �� ���� �� ���̸� 1�� ������̴�.
	

	pVertices[0].vPosition = pPoint[0]; //! ���� ��
	pVertices[1].vPosition = pPoint[1];
	pVertices[2].vPosition = pPoint[2];

	m_SubResourceData.pSysMem = pVertices;

	//! pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�.
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

//TODO ���� ������ ���� ������ �־��ִ� ���� ��
#pragma endregion

#pragma region INDEX_BUFFER
//TODO �ε��� ������ ���� ������ �־��ִ� ���� ��
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; //! �ε��� ���� ���� ä�ﲨ��
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort* pIndices = new _ushort[m_iNumIndices]; //! ���� ������ 65535 �ȳ����ϱ� _ushort

	pIndices[0] = 0; //! �簢�� ���� ������ �ð���� �������� ù��° �ﰢ�� 
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0; 

	//! �簢�� �ϼ�

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

//TODO �ε��� ������ ���� ������ �־��ִ� ���� ��
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoint)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(pPoint)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();
}
