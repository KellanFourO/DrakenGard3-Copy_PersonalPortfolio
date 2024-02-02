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

	//!  For.Prototype_Component_Texture_Sky  #스카이박스텍스처_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

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

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particles/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particles/Fire/Fire.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particles/Frame/Frame.png"), 1))))
		return E_FAIL;

	m_pGameInstance->Add_ParticleTextureTag(TEXT("Prototype_Component_Texture_Snow"));
	m_pGameInstance->Add_ParticleTextureTag(TEXT("Prototype_Component_Texture_Fire"));
	m_pGameInstance->Add_ParticleTextureTag(TEXT("Prototype_Component_Texture_Frame"));

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Blood"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Blood/Blood%d.dds"), 7))))
	//	return E_FAIL;

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Blood/Blood.dds"), 1))))
		return E_FAIL;

	m_pGameInstance->Add_EffectTextureTag(TEXT("Prototype_Component_Texture_Explosion"));

	m_pGameInstance->Add_ParticleTextureTag(TEXT("Prototype_Component_Texture_Blood"));

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_BornFire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BornFire/BornFire.dds"), 1))))
		return E_FAIL;

	m_pGameInstance->Add_ParticleTextureTag(TEXT("Prototype_Component_Texture_BornFire"));

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_CyilnderMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/CyilnderMask.dds"), 1))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_CyilnderMask */
	//if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_CyilnderMask"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/CyilnderMask.dds"), 1))))
	//	return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_BossFireNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BossFire/BossFireNoise.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_BossFireNoiseTest1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BossFire/175.dds"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_BossFireNoiseTest2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BossFire/244.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Trail/fractal_01.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailMask1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Trail/mask_02.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailMask2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Trail/mask_03.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailMask3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Trail/mask_07.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Trail/trail_pl00.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_EnemyHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnemyHP.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_EnemyHPFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnemyHPFrame.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_EnemyBossHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnemyBossHP.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CyilnderMask */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Texture_EnemyBossHPFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnemyBossHPFrame.dds"), 1))))
		return E_FAIL;

	m_pGameInstance->Add_EffectTextureTag(TEXT("Prototype_Component_Texture_BornFire"));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로드하는 중입니다."));

	Engine::MODELDATA* pFilePathData = new Engine::MODELDATA;
	
	_matrix PivotMatrix; //#모델_초기행렬 
	PivotMatrix = XMMatrixIdentity();

	//! ./texconv.exe -m 0 -f R8G8B8A8_UNORM -o C:\Users\PC\Desktop\Jusin_Project\
	\Client\Bin\Resources\Models\Player\ C:\Users\PC\Desktop\Jusin_Project\DrakenGard3_Project_Git\Client\Bin\Resources\Models\Player\*.png
	
	//TODO 플레이어

	lstrcpy(m_szLoadingText, TEXT("플레이어 모델(을) 로드하는 중입니다."));
	
	//////! Player_Body
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("Player"), pFilePathData), PivotMatrix)));
	//
	////! Player_Weapon1
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Weapon1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Player_Weapon1"), pFilePathData), PivotMatrix)));
	
	
	lstrcpy(m_szLoadingText, TEXT("몬스터 모델(을) 로드하는 중입니다."));
	
	////!Prototype_Component_Model_Monster_EN00
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("EN00"), pFilePathData, true), PivotMatrix)));
	
	////!Prototype_Component_Model_Monster_EN00_Weapon
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN00_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("EN00_Weapon"), pFilePathData), PivotMatrix)));

	////!Prototype_Component_Model_Monster_EN01
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("EN01"), pFilePathData, true), PivotMatrix)));

	////!Prototype_Component_Model_Monster_EN01_Weapon
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN01_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("EN01_Weapon"), pFilePathData), PivotMatrix)));

	////!Prototype_Component_Model_Monster_EN01_Weapon_Arrow
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN01_Weapon_Arrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("EN01_Weapon_Arrow"), pFilePathData), PivotMatrix)));

	////!Prototype_Component_Model_Monster_EN70
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN70"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("EN070"), pFilePathData, false), PivotMatrix)));
	
	////!Prototype_Component_Model_Monster_EN070_Weapon
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Monster_EN70_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("EN070_Weapon"), pFilePathData), PivotMatrix)));

	//!Prototype_Component_Model_Boss_EN131
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Boss_EN131"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, *CreateDataPath(TEXT("EN_131"), pFilePathData, false), PivotMatrix)));



	lstrcpy(m_szLoadingText, TEXT("지형 모델(을) 로드하는 중입니다."));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_SkyDome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("SkyDome"), pFilePathData), PivotMatrix), true));


	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_AmmorBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("AmmorBox"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Barricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Barricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BaseRoad"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BaseRoad"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BornFire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BornFire"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Bridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Bridge"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeCliff"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeCliff"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeGate"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BridgeStair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BridgeStair"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBarricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBarricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBox1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBox1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenBox2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenBox2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_BrokenGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BrokenGround"), pFilePathData), PivotMatrix), true));

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken3"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_Broken4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_Broken4"), pFilePathData), PivotMatrix), true));
			
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Building_BrokenSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Building_BrokenSet"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Cannon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Cannon"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_CastleWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("CastleWall"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Door"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Drum"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Drum"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_DrumSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("DrumSet"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_eBuilding"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("eBuilding"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Ground"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Ground"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_IronBarricade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("IronBarricade"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_JuteBag"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("JuteBag"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_JuteBag1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("JuteBag1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Kagaribi"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Kagaribi"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Pass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Pass"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenMedium"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenMedium"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenSmall"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rock_BrokenSmall1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rock_BrokenSmall1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble1"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_Rubble2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Rubble2"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_SkeletalBuilding"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("SkeletalBuilding"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Stage1_Map1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Stage1Map"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Stair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Stair"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_StandRamp"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("StandRamp"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Turret"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Turret"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodBox"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodFlank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodFlank"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodFlank_Broken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodFlank_Broken"), pFilePathData), PivotMatrix), true));

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Environment_WoodSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("WoodSet"), pFilePathData), PivotMatrix), true));
	

	/*Safe_Delete(pFilePathData);*/
	
	//!For.Prototype_Component_VIBuffer_Cube #큐브_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
			CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Terrain #터레인_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
				return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #동적터레인_Add_ProtoType
		if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),
			CVIBuffer_Dynamic_Terrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	//!For.Prototype_Component_VIBuffer_Dynamic_Terrain #동적터레인_Add_ProtoType
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

	//! For.Prototype_Component_VIBuffer_Particle_Point 
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
		CVIBuffer_Model_Instance::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	//! For.Prototype_Component_VIBuffer_Trail
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로드하는 중입니다."));
	//! For.Prototype_Component_Shader_VtxNorTex  #노말셰이더_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	//! For.Prototype_Component_Shader_VtxCube  #큐브셰이더_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	//! For.Prototype_Component_Shader_VtxField #필드셰이더_AddPrototype
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_VtxField"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxField.hlsl"), VTXFIELD::Elements, VTXFIELD::iNumElements))))
		return E_FAIL;
	
	//! For.Prototype_Component_Shader_Model  #모델셰이더_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice,m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)));

	//! For.Prototype_Component_Shader_Model  #모델셰이더_AddPrototype
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)));

	/*! For.Prototype_Component_Shader_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Rect.hlsl"), VTX_PARTICLE_RECT::Elements, VTX_PARTICLE_RECT::iNumElements))))
		return E_FAIL;

	/*! For.Prototype_Component_Shader_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_Particle_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_Point.hlsl"), VTX_PARTICLE_POINT::Elements, VTX_PARTICLE_POINT::iNumElements))))
		return E_FAIL;

	/*! For.Prototype_Component_Shader_EffectMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Shader_EffectMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로드하는 중입니다."));
	//! For.Prototype_Component_Navigation
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Stage1MapNavi.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("리지드바디를(을) 로드하는 중입니다."));
	//! For.Prototype_Component_RigidBody
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("상태머신(을) 로드하는 중입니다."));

	//! For.Prototype_Component_StateMachine
	if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("콜라이더를(을) 로드하는 중입니다."));

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


	lstrcpy(m_szLoadingText, TEXT("플레이어 원형(을) 로드하는 중입니다."));

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
	lstrcpy(m_szLoadingText, TEXT("몬스터 원형(을) 로드하는 중입니다."));
	
	/* For.Prototype_GameObject_Monster_EN00 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_EN00"),
		CMonster_EN00::Create(m_pDevice, m_pContext, eLevel), true, true)))
		return E_FAIL;
	//
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Monster_EN00_Weapon"),
		CMonsterPart_EN00_Weapon::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_EN00 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_EN01"),
		CMonster_EN01::Create(m_pDevice, m_pContext, eLevel), true, true)))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_EN01_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_EN01_Arrow"),
		CEN01_Arrow::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;
	
	/* For.Prototype_PartObject_Monster_EN01_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Monster_EN01_Weapon"),
		CMonsterPart_EN01_Weapon::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;


	/* For.Prototype_GameObject_Monster_EN00 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_EN70"),
		CMonster_EN70::Create(m_pDevice, m_pContext, eLevel), true, true)))
		return E_FAIL;
	//
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Monster_EN70_Weapon"),
		CMonsterPart_EN70_Weapon::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_EN131 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_EN131"),
		CBoss_EN131::Create(m_pDevice, m_pContext, eLevel), true, true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Boss_EN131_Weapon"),
		CBossPart_EN131_Weapon::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PartObject_Boss_EN131_SwingTail"),
		CBossPart_EN131_SwingTail::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EN131_Breath"),
		CEN131_Breath::Create(m_pDevice, m_pContext, eLevel), true)))
		return E_FAIL;

	
	lstrcpy(m_szLoadingText, TEXT("지형 원형(을) 로드하는 중입니다."));

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

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment"), CEnvironment_Object::Create(m_pDevice, m_pContext, eLevel)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_BornFire"), CEnvironment_BornFire::Create(m_pDevice, m_pContext, eLevel)));
	

	lstrcpy(m_szLoadingText, TEXT("이펙트 원형(을) 로드하는 중입니다."));
	
	////!Prototype_Component_Model_Monster_EN01_Weapon_Arrow
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Hanabira"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Hanabira"), pFilePathData), PivotMatrix)));

	m_pGameInstance->Add_EffectMeshTag(TEXT("Prototype_Component_Model_Effect_Hanabira"));

	PivotMatrix = XMMatrixIdentity();//XMMatrixScaling(0.1f, 0.1f, 0.1f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_CrossFrame"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("CrossFrame"), pFilePathData), PivotMatrix)));

	m_pGameInstance->Add_EffectMeshTag(TEXT("Prototype_Component_Model_Effect_CrossFrame"));

	PivotMatrix = XMMatrixIdentity();//XMMatrixScaling(0.1f, 0.1f, 0.1f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_SnowCrossFlow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("SnowCrossFlow"), pFilePathData), PivotMatrix)));

	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 1.f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_Cyilnder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("Cyilnder"), pFilePathData), PivotMatrix)));

	PivotMatrix = XMMatrixScaling(1.5f, 0.5f, 1.f);// * XMMatrixRotationY(XMConvertToRadians(180.0f)); //! 모델의 초기 회전 셋팅
	FAILED_CHECK(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Effect_BossFire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, *CreateDataPath(TEXT("BossFire"), pFilePathData), PivotMatrix)));

	m_pGameInstance->Add_EffectMeshTag(TEXT("Prototype_Component_Model_Effect_SnowCrossFlow"));

	FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeshEffect"), CMeshEffect::Create(m_pDevice, m_pContext, eLevel)));

	/* For.Prototype_GameObject_Particle_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"),
		CParticle_Blue::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Red"),
		CParticle_Red::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Object"),
		CParticle_Object::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"),
		CEffect_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_BornFire"),
		CEffect_BornFire::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Hanabira"),
		CEffect_Hanabira::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_BossBreath"),
		CEffect_BossBreath::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Blood"),
		CEffect_Blood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_EffectTrail"),
		CEffect_Trail::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("UI 원형(을) 로드하는 중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHP"),
		CUI_MonsterHP::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHPFrame"),
		CUI_MonsterHPFrame::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("카메라 원형(을) 로드하는 중입니다."));
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Logo.jpg"), 1))))
		return E_FAIL;

	


	//! For.Prototype_GameObject_BackGround
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice,m_pContext))))
		return E_FAIL;
	
	//! For.Prototype_GameObject_BackGround

	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	CBackGround_Loading::BACKGROUND_DESC	BackGroundDesc = {};

	BackGroundDesc.fX = g_iWinSizeX >> 1;
	BackGroundDesc.fY = g_iWinSizeY >> 1;
	BackGroundDesc.fSizeX = g_iWinSizeX;
	BackGroundDesc.fSizeY = g_iWinSizeY;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BackGroundDesc.fSpeedPerSec = 10.0f;

	

	return Loading_For_Level(LEVEL_GAMEPLAY);
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	return Loading_For_Level(LEVEL_TOOL);
}

Engine::MODELDATA* CLoader::CreateDataPath(wstring strModelName, Engine::MODELDATA* pModelData, _bool bHitAnim)
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

	

	m_pGameInstance->Add_ModelData(strModelName, pModelData);
		 
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
