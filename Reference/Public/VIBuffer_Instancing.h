#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct  
	{
	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual	~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_VIBuffers() override; //! �ν��Ͻ��� ���ε��� �����ϱ����� override �Ѱ��� �� �� �ִ�
	virtual HRESULT Render();

protected:
	ID3D11Buffer*	m_pVBInstance = { nullptr }; //! �ν��Ͻ��� ���� ���۷μ� �ν��Ͻ����۶�� �Ҹ���.
	_uint			m_iInstanceStride = { 0 }; //! �ν��Ͻ� ���� �ϳ��� ũ��� �÷� 16���� ������ ����� ũ���� �ߴ�. �ν��Ͻ� ����� ���� �������
	_uint			m_iNumInstance = { 0 } ; /* !�׸� ��ü 1���� 1���� ��������� �ʿ��ϴٰ� �ߴ�.�ν��Ͻ� ���� 1���� 1���� ��������� ������.*/ //TODO ��, �׸� ��ü ����
	_uint			m_iIndexCountPerInstance = { 0 }; /*!�׸� ��ü�� �ʿ��� �ε����� 6����� ������ ��� 6�̶�� �ϸ� �ȴ�.*/ //TODO �ε��� ������ ��ŭ �Ѱ��� ġ�ڴ�.

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

