#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	CVIBuffer_Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	//TODO override�� ���� �̾߱�
	//! �������̵带 ���̸� �θ�Ŭ������ �Լ��� ȣ�������ʰ�, �ڽ��� �� �Լ��� ȣ���϶�� ������ ��
	
	virtual HRESULT Initialize_Prototype(const _float3 * pPoint);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Cell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoint);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END