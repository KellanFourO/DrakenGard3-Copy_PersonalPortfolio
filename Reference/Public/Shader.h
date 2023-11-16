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
	virtual HRESULT Initialize_Prototype(const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements); //! 프로토타입 패턴을 사용한다는 것을 알 수 있다
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);

private:
	//!내가 빌드하여 컴파일한 셰이더 파일 하나를 대표하는 컴객체
	D3DX11_TECHNIQUE_DESC		m_TechniqueDesc;
	ID3DX11Effect*				m_pEffect = { nullptr };
	vector<ID3D11InputLayout*>	m_InputLayouts;

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

