#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);

	//TODO ���۸� ���� �����ص� �Ǵ�����
	//! �ؽ�ó�� �ʿ��Ҷ����� �ε��ϴ� �� ����, �ε����� �̸� �ް� ��������� ����ϴ°� ���Ƽ�.
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	
	//TODO �Ʒ� �Լ��� ȣ���ϸ� �׸���� ����� ������.
	//! ������ � �������ۿ� �ε������۸� ����ؼ� �׸���� ���� �������� �ʾҴ�
	//! �׷��� ��ġ�� �������ۿ� �ε������۸� ���ε� ���ִ� �۾��� �ʿ��ϴ�. -> Bind_VIBuffer()
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	
	return S_OK;
}

HRESULT CVIBuffer::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;


	//TODO �Ʒ��� ������ �ʿ��ؼ� ���� ���ۼ�
	//m_pContext->IASetVertexBuffers(); 
	//m_pContext->IASetIndexBuffer();
	//m_pContext->IASetPrimitiveTopology(); //! � ���·� �׸� �ų�

	return E_NOTIMPL;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
	//TODO DX9�� DX11������ ���ۻ��� ����
	//! DX9������ ���۸� ���� ���� �� ��, ����� ȣ���ؼ� ���� ä�� �־�����.
	//! DX11������ ���۸� �Ҵ��� ������ ���� ä���ִ� ������� �ٲ����.
	
	//! Why? ���������� ���, �� ����� ȣ���� �� �ý��ۿ� ���� ���ϸ� �ش�
	//! �׷��� 11���ʹ� ��, ����� ȣ���ϴ� �� �Ұ����ϰ� �������ȴ�.
	
	//! Result = ��, ��, ��� �Ұ����ϴ� �Ҵ��� �� ���� �� ä���ذ�

	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, ppBuffer);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
