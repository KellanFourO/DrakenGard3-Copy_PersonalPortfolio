#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pImguiMgr(CImgui_Manager::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImguiMgr);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.iBackBufferSizeX = g_iWinSizeX;
	GraphicDesc.iBackBufferSizeY = g_iWinSizeY;

	if(FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_ForStaticLevel()))
		return E_FAIL;

	if(FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(Initialize_Imgui()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

	if (m_pImguiMgr->Get_Ready())
		m_pImguiMgr->Tick(fTimeDelta);

}

HRESULT CMainApp::Render()
{

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	//TODO 추후, 그려야할 모델들을 그린다.
	
	m_pGameInstance->Render_Engine();

	if (m_pImguiMgr->Get_Ready())
		m_pImguiMgr->Render();
	
	m_pGameInstance->Present();
	

	return S_OK;


}

HRESULT CMainApp::Open_Level(LEVEL eStartLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 무조건 로딩레벨부터 시작ㅇ르 할꺼야 .*/
	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID);
	if (nullptr == pLevel)
		return E_FAIL;
	
	m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_ForStaticLevel()
{
	/* For.Prototype_Component_VIBuffer_Rect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxPosTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsli"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Initialize_Imgui()
{

	if (nullptr == m_pImguiMgr)
	{
		MSG_BOX("Imgui GetInstance Failed");
		return E_FAIL;
	}

	if (FAILED(m_pImguiMgr->Initialize(g_hWnd,m_pDevice, m_pContext)))
	{
		MSG_BOX("Imgui Initialize Failed");
		return E_FAIL;
	}

	return S_OK;
}


CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	/*  내 멤버변수 정리하기 */
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pImguiMgr);
	

	CImgui_Manager::GetInstance()->DestroyInstance();
	CGameInstance::Release_Engine();
}

