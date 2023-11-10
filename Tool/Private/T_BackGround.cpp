#include "../Default/stdafx.h"
#include "T_BackGround.h"
#include "GameInstance.h"

CT_BackGround::CT_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
}

CT_BackGround::CT_BackGround(const CT_BackGround& rhs)
	:CGameObject(rhs)
{
}

HRESULT CT_BackGround::Initialize_Prototype()
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	return S_OK;
}

HRESULT CT_BackGround::Initialize(void* pArg)
{
	return S_OK;
}

void CT_BackGround::Priority_Tick(_float fTimeDelta)
{
	int i = 0;
}

void CT_BackGround::Tick(_float fTimeDelta)
{
	int i = 0;
}

void CT_BackGround::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CT_BackGround::Render() const
{
	return S_OK;
}

CT_BackGround* CT_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CT_BackGround* pInstance = new CT_BackGround(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CT_BackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CT_BackGround::Clone(void* pArg)
{
	CT_BackGround* pInstance = new CT_BackGround(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CT_BackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CT_BackGround::Free()
{
	__super::Free();
}
