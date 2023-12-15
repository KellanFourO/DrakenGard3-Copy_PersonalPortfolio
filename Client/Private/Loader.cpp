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


#include <process.h> //! �����带 ����ϱ����� ��� �߰�

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
	//! �����尡 �ٸ� �Ӱ�޸𸮿����� ħ���ؼ� ����� ������ �������� �İ�ü�� �ʱ�ȭ���ش�.
	//! ����̽� �İ�ü���� ��Ȥ���� �ٸ� �����忡�� ������� �İ�ü�� ����ϴ� ��찡 �־� �˼����� ���׸� ����Ų��.
	//! �׷��� Ư�� �����尡 ���۵ɽ� DX���� �����س��� �İ�ü���� �� �����忡�� ����� �� �ֵ��� �ٸ� �����忡�� ��� �� �̾��ٸ� ���� ������ �ʱ�ȭ ���ִ� �Լ��̴�.
	
	//! ��ǥ������ ���⼭ ���� ������ �����״�., �ؽ�ó�� �ҷ��ö� ù��° ���� �ؽ�ó�� �ε��� �� ������ ������ ���� �߻��߾���.
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* � ������ �ڿ��� �ε��ؾ��ϴ���? */
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* �����带 �����Ѵ�. */
	/* LoadingThread : ������ �����尡 ���� ���� ȣ���ؾ��� �Լ� */
	/* this : �� �Լ��� ȣ���ϸ鼭 ���ڷ� ������ ������. */
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
	/* �����÷��� ������ �ʿ��� �ڿ��� �ε�����. */
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε��ϴ� ���Դϴ�."));
	//!  For.Prototype_Component_Texture_Terrain_Mask  #�ͷ����ؽ�ó_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	//!  For.Prototype_Component_Texture_Terrain_Mask  #�ͷ��θ���ũ�ؽ�ó_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	//!  For.Prototype_Component_Texture_Terrain_Brush  #�ͷ��κ귯���ؽ�ó_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�𵨸�(��) �ε��ϴ� ���Դϴ�."));


	CModel::ModelData* pFilePathData = new CModel::ModelData;
	////!For.Prototype_Component_Model_Fiona #�ǿ���_Add_ProtoType
	_matrix PivotMatrix; //#��_�ʱ���� 
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("Fiona"), pFilePathData), PivotMatrix)));
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/pl00.fbx", PivotMatrix)));

	//!For.Prototype_Component_Model_ForkLift #��ũ����Ʈ_Add_ProtoType
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("ForkLift"), pFilePathData), PivotMatrix)));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_TestTree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Tree"), pFilePathData), PivotMatrix)));

	//Safe_Delete(pFilePathData);

	//!For.Prototype_Component_VIBuffer_Terrain #�ͷ���_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
				return E_FAIL;
	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #�����ͷ���_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
			CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		

	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε��ϴ� ���Դϴ�."));
	//! For.Prototype_Component_Shader_VtxNorTex  #�븻���̴�_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	
	//! For.Prototype_Component_Shader_Model  #�𵨼��̴�_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice,m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));

	//! For.Prototype_Component_Shader_Model  #�𵨼��̴�_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));

	lstrcpy(m_szLoadingText, TEXT("�׺���̼�(��) �ε��ϴ� ���Դϴ�."));
	//! For.Prototype_Component_Navigation
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("������ü��(��) �ε��ϴ� ���Դϴ�."));

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

	//! For.Prototype_GameObject_Monster #����_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice,m_pContext, eLevel), true));

	//! For.Prototype_GameObject_TestTree #�׽�ƮƮ��_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestTree"), CTestTree::Create(m_pDevice, m_pContext, eLevel), true));

	//! For.Prototype_GameObject_ForkLift #��ũ����Ʈ_AddPrototype
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

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;

}

HRESULT CLoader::Loading_For_Logo_Level()
{
	/* �ΰ� ������ �ʿ��� �ڿ��� �ε�����. */
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_Component_Texture_Logo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�𵨸�(��) �ε��ϴ� ���Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε��ϴ� ���Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("������ü��(��) �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_GameObject_BackGround
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

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
