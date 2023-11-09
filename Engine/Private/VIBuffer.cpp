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

	//TODO 버퍼를 얕은 복사해도 되는이유
	//! 텍스처를 필요할때마다 로드하는 것 보다, 로딩간에 미리 받고 얕은복사로 사용하는게 나아서.
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
	
	//TODO 아래 함수만 호출하면 그리라는 명령은 끝났다.
	//! 하지만 어떤 정점버퍼와 인덱스버퍼를 사용해서 그리라는 것을 정해주지 않았다
	//! 그래서 장치에 정점버퍼와 인덱스버퍼를 바인딩 해주는 작업이 필요하다. -> Bind_VIBuffer()
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	
	return S_OK;
}

HRESULT CVIBuffer::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;


	//TODO 아래는 설명이 필요해서 아직 미작성
	//m_pContext->IASetVertexBuffers(); 
	//m_pContext->IASetIndexBuffer();
	//m_pContext->IASetPrimitiveTopology(); //! 어떤 형태로 그릴 거냐

	return E_NOTIMPL;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
	//TODO DX9과 DX11에서의 버퍼생성 차이
	//! DX9에서는 버퍼를 먼저 생성 후 락, 언락을 호출해서 값을 채워 넣었었다.
	//! DX11에서는 버퍼를 할당할 때부터 값을 채워주는 방식으로 바뀌었다.
	
	//! Why? 정적버퍼의 경우, 락 언락을 호출할 시 시스템에 많은 부하를 준다
	//! 그래서 11부터는 락, 언락을 호출하는 걸 불가능하게 만들어버렸다.
	
	//! Result = 즉, 락, 언락 불가능하니 할당할 때 부터 값 채워준것

	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, ppBuffer);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
