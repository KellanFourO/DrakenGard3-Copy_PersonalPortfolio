#include "stdafx.h"
#include "Level_Tool.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

 	CImgui_Manager::GetInstance()->Set_Ready(true);
	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("툴 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}



CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
	//CImgui_Manager::GetInstance()->DestroyInstance();
}
