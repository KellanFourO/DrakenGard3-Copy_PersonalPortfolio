#pragma once

#include "Component.h"

//TODO 트랜스폼 컴객체의 역할
//! 객체들의 월드 상태를 표현하기 위한 데이터를 가진다. ( 월드행렬 )
//! 해당 월드 상에서의 변환을 위한 기능을 가진다.

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
	virtual CComponent* Clone(void* pArg) override; //! 복사를 강제한다
	virtual void Free() override;
};

END

