#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sprite final : public CVIBuffer
{
private:
	CVIBuffer_Sprite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs);
	virtual ~CVIBuffer_Sprite() = default;

public:
	//TODO override�� ���� �̾߱�
	//! �������̵带 ���̸� �θ�Ŭ������ �Լ��� ȣ�������ʰ�, �ڽ��� �� �Լ��� ȣ���϶�� ������ ��
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


	_int	m_iVerCount = 0; //! ���� ĭ ����
	_int	m_iHorCount = 0; //! ���� ĭ ����

public:
	static CVIBuffer_Sprite* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END