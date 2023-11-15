#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath)
{
	//!strShaderFilePath 경로에 작성되어 있는 hlsl언어 번역 빌드하여 ID3DX11Effect라는 녀석을 만들자

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	return nullptr;
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
