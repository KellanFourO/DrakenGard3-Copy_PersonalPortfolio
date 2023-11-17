#pragma once

#include "Component.h"

//TODO Ʈ������ �İ�ü�� ����
//! ��ü���� ���� ���¸� ǥ���ϱ� ���� �����͸� ������. ( ������� )
//! �ش� ���� �󿡼��� ��ȯ�� ���� ����� ������.

BEGIN(Engine)

class CTransform final : public CComponent
{
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	
private:
	_float4x4			m_WorldMatrix;

public:
	static	CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override; //! ���縦 �����Ѵ�
	virtual void Free() override;
};

END

