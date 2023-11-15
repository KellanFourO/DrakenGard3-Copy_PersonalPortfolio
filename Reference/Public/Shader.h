#pragma once

#include "Component.h"
BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
protected:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype() override; //! 프로토타입 패턴을 사용한다는 것을 알 수 있다
	virtual HRESULT Initialize(void* pArg) override;

private:
	ID3D11VertexShader*	m_VertexShader = { nullptr };
	ID3D11PixelShader*  m_PixelShader = { nullptr };
	ID3D11InputLayout*  m_layout = { nullptr};
	
	PASS_DESC	m_PassDesc = { 0 };

public:
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

