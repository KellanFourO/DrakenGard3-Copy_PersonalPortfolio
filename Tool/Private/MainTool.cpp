#include "stdafx.h"

#include "MainTool.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"


CMainTool::CMainTool()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainTool::Initialize()
{
	GRAPHIC_DESC		GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.iBackBufferSizeX = g_iWinSizeX;
	GraphicDesc.iBackBufferSizeY = g_iWinSizeY;

	

	if (FAILED(m_pGameInstance->Initialize_Engine(0,GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	m_pImguiManager = CImgui_Manager::Create(m_pDevice,m_pContext);

	//if (FAILED(Open_Tool(TOOL_EFFECT)))
	//	return E_FAIL;

	return S_OK;
}

void CMainTool::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pImguiManager->Tick(fTimeDelta);
}

HRESULT CMainTool::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	//TODO 추후, 그려야할 모델들을 그린다.

	m_pGameInstance->Render_Engine();
	
	m_pImguiManager->Render();
	
	m_pGameInstance->Present();

	return S_OK;
}



HRESULT CMainTool::Open_Tool(TOOL eStartToolID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 무조건 로딩레벨부터 시작ㅇ르 할꺼야 .*/
	//CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID);
	//if (nullptr == pLevel)
	//	return E_FAIL;
	//
	//m_pGameInstance->Open_Level(pLevel);

	return S_OK;
}

CMainTool* CMainTool::Create()
{
	CMainTool* pInstance = new CMainTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainTool::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pImguiManager);
	CGameInstance::Release_Engine();
}
