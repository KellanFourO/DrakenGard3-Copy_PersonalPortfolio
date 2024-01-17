 #include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

//TODO GameObject
#include "Camera_Dynamic.h"
#include "Camera_Target.h"

//TODO Tool
#include "Camera_MapTool.h"
#include "Field.h"
#include "Imgui_Manager.h"

#include "AnimObjects.h"
#include "NonAnimObjects.h"

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

	//!  For.Prototype_Component_Texture_Sky  #��ī�̹ڽ��ؽ�ó_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

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

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�𵨸�(��) �ε��ϴ� ���Դϴ�."));

	CModel::ModelData* pFilePathData = new CModel::ModelData;
	
	_matrix PivotMatrix; //#��_�ʱ���� 
	PivotMatrix = XMMatrixIdentity();

	//! ./texconv.exe -m 0 -f R8G8B8A8_UNORM -o C:\Users\PC\Desktop\Jusin_Project\
	\Client\Bin\Resources\Models\Player\ C:\Users\PC\Desktop\Jusin_Project\DrakenGard3_Project_Git\Client\Bin\Resources\Models\Player\*.png
	
	//TODO �÷��̾�

	lstrcpy(m_szLoadingText, TEXT("�÷��̾� ��(��) �ε��ϴ� ���Դϴ�."));
	
	//////! Player_Body
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("Player"), pFilePathData), PivotMatrix)));
	//
	////! Player_Weapon1
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Weapon1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Player_Weapon1"), pFilePathData), PivotMatrix)));
	
	
	lstrcpy(m_szLoadingText, TEXT("���� ��(��) �ε��ϴ� ���Դϴ�."));
	
	////!Prototype_Component_Model_Monster_EN00
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("EN00"), pFilePathData, true), PivotMatrix)));
	//
	////!Prototype_Component_Model_Monster_EN00
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN00_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("EN00_Weapon"), pFilePathData), PivotMatrix)));


	lstrcpy(m_szLoadingText, TEXT("���� ��(��) �ε��ϴ� ���Դϴ�."));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_SkyDome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("SkyDome"), pFilePathData), PivotMatrix), true));


	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_AmmorBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("AmmorBox"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Barricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Barricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BaseRoad"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BaseRoad"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BornFire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BornFire"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Bridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Bridge"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeCliff"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeCliff"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeGate"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeStair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeStair"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBarricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBarricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBox1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBox1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBox2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBox2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenGround"), pFilePathData), PivotMatrix), true));

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken3"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken4"), pFilePathData), PivotMatrix), true));
			
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_BrokenSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_BrokenSet"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Cannon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Cannon"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_CastleWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("CastleWall"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Door"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Drum"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Drum"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_DrumSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("DrumSet"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_eBuilding"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("eBuilding"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Ground"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Ground"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_IronBarricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("IronBarricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_JuteBag"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("JuteBag"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_JuteBag1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("JuteBag1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Kagaribi"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Kagaribi"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Pass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Pass"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenMedium"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenMedium"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenSmall"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenSmall1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenSmall1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_SkeletalBuilding"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("SkeletalBuilding"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Stage1_Map1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Stage1Map"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Stair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Stair"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_StandRamp"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("StandRamp"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Turret"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Turret"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodBox"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodFlank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodFlank"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodFlank_Broken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodFlank_Broken"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! ���� �ʱ� ȸ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodSet"), pFilePathData), PivotMatrix), true));
	

	Safe_Delete(pFilePathData);
	
	//!For.Prototype_Component_VIBuffer_Cube #ť��_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
			CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Terrain #�ͷ���_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
				return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #�����ͷ���_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
			CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #�����ͷ���_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Field"),
			CVIBuffer_Field::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	//! For.Prototype_Component_VIBuffer_Particle_Rect 
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
			CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 100))))
			return E_FAIL;

	//! For.Prototype_Component_VIBuffer_Particle_Point 
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;
		

	lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε��ϴ� ���Դϴ�."));
	//! For.Prototype_Component_Shader_VtxNorTex  #�븻���̴�_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	//! For.Prototype_Component_Shader_VtxCube  #ť����̴�_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	//! For.Prototype_Component_Shader_VtxField #�ʵ���̴�_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxField"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxField.hlsl"), VTXFIELD::Elements, VTXFIELD::iNumElements))))
		return E_FAIL;
	
	//! For.Prototype_Component_Shader_Model  #�𵨼��̴�_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice,m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));

	//! For.Prototype_Component_Shader_Model  #�𵨼��̴�_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));

	/* For.Prototype_Component_Shader_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�׺���̼�(��) �ε��ϴ� ���Դϴ�."));
	//! For.Prototype_Component_Navigation
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Stage1MapNavi.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("������ٵ�(��) �ε��ϴ� ���Դϴ�."));
	//! For.Prototype_Component_RigidBody
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���¸ӽ�(��) �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_Component_StateMachine
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ݶ��̴���(��) �ε��ϴ� ���Դϴ�."));

	//! For.Prototype_Component_Collider_AABB
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	//! For.Prototype_Component_Collider_OBB
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	//! For.Prototype_Component_Collider_Sphere
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_Sphere))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("�÷��̾� ����(��) �ε��ϴ� ���Դϴ�."));

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext), true, true)))
		return E_FAIL;
	
	/* For.Prototype_PartObject_PlayerBody */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_PlayerBody"),
		CPlayerPart_Body::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;
	
	/* For.Prototype_PartObject_PlayerWeapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_PlayerWeapon"),
		CPlayerPart_Weapon::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;
	//
	lstrcpy(m_szLoadingText, TEXT("���� ����(��) �ε��ϴ� ���Դϴ�."));
	
	/* For.Prototype_GameObject_Monster_EN00 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_EN00"),
		CMonster_EN00::Create(m_pDevice, m_pContext, eLevel), true, true)))
		return E_FAIL;
	//
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Monster_EN00_Weapon"),
		CMonsterPart_EN00_Weapon::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� ����(��) �ε��ϴ� ���Դϴ�."));

	/* For.Protytype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Protytype_GameObject_SkyDome*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyDome"),
		CSkyDome::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	///* For.Protytype_GameObject_TestSnow */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestSnow"),
	//	CTestSnow::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Terrain"),
		CDynamic_Terrain::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("����Ʈ ����(��) �ε��ϴ� ���Դϴ�."));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment"), CEnvironment_Object::Create(m_pDevice,m_pContext, eLevel)));

	/* For.Prototype_GameObject_Particle_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"),
		CParticle_Blue::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"),
		CParticle_Red::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"),
		CEffect_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("ī�޶� ����(��) �ε��ϴ� ���Դϴ�."));
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Target"),
		CCamera_Target::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

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

		/* For.Prototype_GameObject_Field */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Field"),
			CField::Create(m_pDevice, m_pContext, eLevel))))
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

CModel::ModelData* CLoader::CreateDataPath(wstring strModelName, CModel::ModelData* pModelData, _bool bHitAnim)
{
	wstring strDataPath = TEXT("../Bin/Resources/Models/") + strModelName + TEXT("/");
	wstring strBoneEXT = TEXT(".bone");
	wstring strMeshEXT = TEXT(".mesh");
	wstring strMaterialEXT = TEXT(".mat");
	wstring strAnimationEXT = TEXT(".anim");
	wstring strHitAnimationEXT = TEXT(".hitanim");
	

	pModelData->strBoneDataPath = strDataPath + strModelName + strBoneEXT;
	pModelData->strMeshDataPath = strDataPath + strModelName + strMeshEXT;
	pModelData->strMaterialDataPath = strDataPath + strModelName + strMaterialEXT;
	pModelData->strAnimationDataPath = strDataPath + strModelName + strAnimationEXT;

	if (true == bHitAnim)
	{
		pModelData->strHitAnimationDataPath = strDataPath + strModelName + strHitAnimationEXT;
	}

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
