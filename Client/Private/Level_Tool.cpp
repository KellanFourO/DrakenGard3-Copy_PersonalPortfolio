#include "stdafx.h"
#include "Level_Tool.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Field.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	
	
	//m_pGameInstance->Add_LayerTag(TEXT("Layer_Camera"));

	m_pGameInstance->Add_LayerTag(TEXT("Layer_Player"));
	m_pGameInstance->Add_LayerTag(TEXT("Layer_Monster"));
	m_pGameInstance->Add_LayerTag(TEXT("Layer_Boss"));
	m_pGameInstance->Add_LayerTag(TEXT("Layer_Effect"));
	m_pGameInstance->Add_LayerTag(TEXT("Layer_BackGround"));


	if (FAILED(Ready_Imgui()))
	{
		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);
	
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		if(GetAsyncKeyState(VK_F5) & 0x8000)
			if (!m_bModeChange)
			{
				m_pGameInstance->UseFullScreen(true);
			}
			else
			{
				m_pGameInstance->UseFullScreen(false);
			}

			m_bModeChange = !m_bModeChange;
	}
	
	m_pImguiManager->Tick(fTimeDelta);

}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("툴 레벨입니다."));
		
	m_pImguiManager->Render();

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Imgui()
{
	m_pImguiManager = CImgui_Manager::GetInstance();
	m_pImguiManager->AddRef();
	
	if(nullptr == m_pImguiManager)
		return E_FAIL;

	if(FAILED(m_pImguiManager->Initialize(m_pDevice,m_pContext)))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Dynamic::DYNAMIC_CAMERA_DESC	Desc = {};

	Desc.fMouseSensitivity = 0.05f;
	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovY = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Camera_MapTool"), &Desc, (CGameObject**)m_pImguiManager->Get_Cam())))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_TestTree"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Effect(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Red"))))
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
	Safe_Release(m_pImguiManager);
	CImgui_Manager::GetInstance()->DestroyInstance();
}
