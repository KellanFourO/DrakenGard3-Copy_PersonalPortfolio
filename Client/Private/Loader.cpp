#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

//TODO GameObject
#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Dynamic_Terrain.h"
#include "Monster.h"
#include "TestTree.h"
#include "ForkLift.h"
#include "Player.h"


//TODO Tool
#include "Camera_MapTool.h"
#include "Imgui_Manager.h"


#include <process.h> //! 스레드를 사용하기위한 헤더 추가

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY LoadingThread(void* pArg)
{
	//! 쓰레드가 다른 임계메모리영역을 침범해서 생기는 에러를 막기위해 컴객체를 초기화해준다.
	//! 디바이스 컴객체들은 간혹가다 다른 스레드에서 사용중인 컴객체를 사용하는 경우가 있어 알수없는 버그를 일으킨다.
	//! 그래서 특정 스레드가 시작될시 DX에서 셋팅해놨던 컴객체들을 이 스레드에서 사용할 수 있도록 다른 스레드에서 사용 중 이었다면 전부 꺼내서 초기화 해주는 함수이다.
	
	//! 대표적으로 여기서 많은 에러를 일으켰다., 텍스처를 불러올때 첫번째 이후 텍스처를 로드할 때 터지는 현상이 많이 발생했었다.
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* 어떤 레벨의 자원을 로드해야하는지? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성한다. */
	/* LoadingThread : 생성한 스레드가 가장 먼저 호출해야할 함수 */
	/* this : 위 함수를 호출하면서 인자로 전달할 데이터. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

void CLoader::Print_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = 0;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level(LEVEL eLevel)
{
	/* 게임플레이 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));
	//!  For.Prototype_Component_Texture_Terrain_Mask  #터레인텍스처_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	//!  For.Prototype_Component_Texture_Terrain_Mask  #터레인마스크텍스처_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	//!  For.Prototype_Component_Texture_Terrain_Brush  #터레인브러쉬텍스처_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));


	CModel::ModelData* pFilePathData = new CModel::ModelData;
	////!For.Prototype_Component_Model_Fiona #피오나_Add_ProtoType
	_matrix PivotMatrix; //#모델_초기행렬 
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("Fiona"), pFilePathData), PivotMatrix)));
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/pl00.fbx", PivotMatrix)));

	//!For.Prototype_Component_Model_ForkLift #포크리프트_Add_ProtoType
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("ForkLift"), pFilePathData), PivotMatrix)));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_TestTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Tree"), pFilePathData), PivotMatrix)));

	//Safe_Delete(pFilePathData);

	//!For.Prototype_Component_VIBuffer_Terrain #터레인_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
				return E_FAIL;
	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #동적터레인_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
			CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	//! For.Prototype_Component_Shader_VtxNorTex  #노말셰이더_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	
	//! For.Prototype_Component_Shader_Model  #모델셰이더_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice,m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));

	//! For.Prototype_Component_Shader_Model  #모델셰이더_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));

	lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로드하는 중입니다."));
	//! For.Prototype_Component_Navigation
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Terrain"),
		CDynamic_Terrain::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	//! For.Prototype_GameObject_Monster #몬스터_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice,m_pContext, eLevel), true));

	//! For.Prototype_GameObject_TestTree #테스트트리_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestTree"), CTestTree::Create(m_pDevice, m_pContext, eLevel), true));

	//! For.Prototype_GameObject_ForkLift #포크리프트_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext, eLevel), true));

	
		

	
	switch (eLevel)
	{
	/* For.Prototype_GameObject_Camera_Dynamic */
	case Client::LEVEL_GAMEPLAY:
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
			CCamera_Dynamic::Create(m_pDevice, m_pContext, eLevel))))
			return E_FAIL;
		break;
	/* For.Prototype_GameObject_Camera_MapTool */
	case Client::LEVEL_TOOL:
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_MapTool"),
			CCamera_MapTool::Create(m_pDevice, m_pContext, eLevel))))
			return E_FAIL;
		break;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;

}

HRESULT CLoader::Loading_For_Logo_Level()
{
	/* 로고 레벨에 필요한 자원을 로드하자. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로드하는 중입니다."));

	//! For.Prototype_Component_Texture_Logo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체를(을) 로드하는 중입니다."));

	//! For.Prototype_GameObject_BackGround
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	

	return Loading_For_Level(LEVEL_GAMEPLAY);
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	return Loading_For_Level(LEVEL_TOOL);
}

CModel::ModelData* CLoader::CreateDataPath(wstring strModelName, CModel::ModelData* pModelData)
{
	wstring strDataPath = TEXT("../Bin/DataFiles/Model/");
	wstring strBoneEXT = TEXT(".bone");
	wstring strMeshEXT = TEXT(".mesh");
	wstring strMaterialEXT = TEXT(".mat");
	wstring strAnimationEXT = TEXT(".anim");

	

	pModelData->strBoneDataPath = strDataPath + strModelName + strBoneEXT;
	pModelData->strMeshDataPath = strDataPath + strModelName + strMeshEXT;
	pModelData->strMaterialDataPath = strDataPath + strModelName + strMaterialEXT;
	pModelData->strAnimationDataPath = strDataPath + strModelName + strAnimationEXT;

	return pModelData;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
