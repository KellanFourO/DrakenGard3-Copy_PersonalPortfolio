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
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath); //! ������Ÿ�� ������ ����Ѵٴ� ���� �� �� �ִ�
	virtual HRESULT Initialize(void* pArg) override;

private:
	//!���� �����Ͽ� �������� ���̴� ���� �ϳ��� ��ǥ�ϴ� �İ�ü
	ID3DX11Effect*		m_pEffect = { nullptr };

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

