#include "stdafx.h"
#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Tool.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	
	/* ���ν������ ���� �ε��Ѵ�. */
	/* �ε��� �ڿ��� �ε��Ѵ�. */
	/* �ε��������� ��������� ��ü���� �����Ѵ�.(���, �Ϸ���Ʈ, �ε���) */

	/* �߰����� �����带 �����Ͽ� eNextLevelID�� �ʿ��� �ڿ����� �ε��Ѵ�. */
	

	CBackGround_Loading::BACKGROUND_DESC	BackGroundDesc = {};

	BackGroundDesc.fX = g_iWinSizeX >> 1;
	BackGroundDesc.fY = g_iWinSizeY >> 1;
	BackGroundDesc.fSizeX = g_iWinSizeX;
	BackGroundDesc.fSizeY = g_iWinSizeY;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BackGroundDesc.fSpeedPerSec = 10.0f;

	CGameObject* pGameObject = { nullptr};

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_BackGround_Loading"), &BackGroundDesc, &pGameObject)))
		return E_FAIL;

	m_pLoading = dynamic_cast<CBackGround_Loading*>(pGameObject);

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;


	m_pGameInstance->Play_BGM(L"BGM", L"LoadingBGM3.wav", 1.f);

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	if (true == m_pLoader->isFinished())
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_RETURN) & 0x80)
		{
			CLevel* pNewLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
				return;

		
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Print_LoadingText();
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	
	Safe_Release(m_pLoader);

	
}
