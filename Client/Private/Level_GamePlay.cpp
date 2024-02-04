#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
//#include "SkyBox.h"
#include "Dynamic_Terrain.h"
#include "Player.h"
#include "Camera_Target.h"
#include "Environment_Object.h"
#include "Effect_Blood.h"
#include "Event_Boss1Apeear.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Event()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Collider()))
		return E_FAIL; 


 	


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	LIGHT_DESC			LightDesc{};
	
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
 	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
 		return E_FAIL;
// 	
 	//ZeroMemory(&LightDesc, sizeof LightDesc);
// 	/ /
 	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
 	//LightDesc.vPosition = _float4(45.f, 10.f, 35.f, 1.f);
 	//LightDesc.fRange = 20.f;
 	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
 	//LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
 	//LightDesc.vSpecular = LightDesc.vDiffuse;
 	//
 	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
 	//	return E_FAIL;
// 	
// 	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
// 	LightDesc.vPosition = _float4(50.f, 3.f, 30.f, 1.f);
// 	LightDesc.fRange = 20.f;
// 	LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.0f, 1.f);
// 	LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
// 	LightDesc.vSpecular = LightDesc.vDiffuse;
// 	
// 	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
// 		return E_FAIL;
// 	
// 	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
// 		return E_FAIL;
// 	
// 	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
// 	LightDesc.vPosition = _float4(70.f, 10.f, 30.f, 1.f);
// 	LightDesc.fRange = 20.f;
// 	LightDesc.vDiffuse = _float4(1.f, 0.0f, 1.f, 1.f);
// 	LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.4f, 1.f);
// 	LightDesc.vSpecular = LightDesc.vDiffuse;
// 	
// 	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
// 		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Target::TARGET_CAMERA_DESC Desc = {};

	Desc.fMouseSensitivity = 0.05f;
	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovY = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(LEVEL_GAMEPLAY));

	if (nullptr == pPlayer)
		return E_FAIL;

	Desc.pTarget = pPlayer;

	CGameObject* pTarget = nullptr;

	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY,strLayerTag,TEXT("Prototype_GameObject_Camera_Target"), &Desc, &pTarget)))
		return E_FAIL;

	
	//pPlayer->Set_Cam(dynamic_cast<CCamera_Target*>(pTarget));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag)
{

	CGameObject::GAMEOBJECT_DESC Desc;

	Desc.iLevelIndex = LEVEL_GAMEPLAY;

	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &Desc, &pGameObject)))
		return E_FAIL;

	pGameObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(43.88f, 0.f, 18.55f, 1.f));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Red"))))
	//	return E_FAIL;

	_vector vPos = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	//for (size_t i = 0; i < 30; i++)
	//{
		//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion"))))
		//	return E_FAIL;
		//_bool bType = false;
		//
		//if(i % 2 == 0)
		//	bType = true;
		//CEffect_Blood::EFFECT_DESC Desc;
		//
		//Desc.fLifeTime = 5.f;
		//Desc.fPlaySpeed = 1.f;
		//XMStoreFloat4(&Desc.vCreatePos, XMVectorSet(45.f, 1.f, 35.f, 1.f));
		////XMStoreFloat3(&Desc.vDir, -m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		//Desc.vScale = _float3{ 1.f, 1.f, 1.f };
		//
		//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Blood"), &Desc);
	
	//}

	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//for (_int i = 0; i < 5; ++i)
	//{
	//	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_EN00"))))
	//	return E_FAIL;
	//}
	
	json LoadJson;

	//CJson_Utility::Load_Json("../Bin/DataFiles/39_Anim.json", LoadJson);
	//CJson_Utility::Load_Json("../Bin/DataFiles/41_Anim.json", LoadJson);
	CJson_Utility::Load_Json("../Bin/DataFiles/43_Anim.json", LoadJson);

	_int LoadSize = LoadJson.size();

	for (_int i = 0; i < LoadSize; ++i)
	{
		CGameObject* pGameObject = nullptr;

		
		CGameObject::GAMEOBJECT_DESC Desc;
		Desc.iCellIndex = LoadJson[i]["CellIndex"];
		
		
		wstring LayerTag = ConvertStrToWstr(LoadJson[i]["LayerTag"]);
		wstring ObjectTag = ConvertStrToWstr(LoadJson[i]["ObjectTag"]);


		if (ObjectTag == TEXT("Prototype_GameObject_Monster_EN00"))
		{
			Desc.fSpeedPerSec = 1.8f;
			Desc.fRotationPerSec = XMConvertToRadians(60.f);
		}
		else if (ObjectTag == TEXT("Prototype_GameObject_Monster_EN70"))
		{
			Desc.fSpeedPerSec = 3.f;
			Desc.fRotationPerSec = XMConvertToRadians(90.f);
		}

		else if (ObjectTag == TEXT("Prototype_GameObject_Monster_EN01"))
		{
			Desc.fSpeedPerSec = 1.3f;
			Desc.fRotationPerSec = XMConvertToRadians(60.f);
		}

		
		
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, LayerTag, ObjectTag, &Desc, &pGameObject)))
			return E_FAIL;
			
		pGameObject->Set_CellIndex(LoadJson[i]["CellIndex"]);

		const json& TransformJson = LoadJson[i]["Component"]["Transform"];

		_float4x4 WorldMatrix;

		for (_int i = 0; i < 4; ++i)
		{
			for (_int j = 0; j < 4; ++j)
			{
				WorldMatrix.m[i][j] = TransformJson[i][j];
			}
		}

		
		pGameObject->Get_Transform()->Set_WorldFloat4x4(WorldMatrix);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	
 	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
 	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_SkyDome"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_TestSnow"))))
	//	return E_FAIL;

	json LoadJson;

	CJson_Utility::Load_Json("../Bin/DataFiles/44_NonAnim.json", LoadJson);

	_int LoadSize = LoadJson.size();

	for (_int i = 0; i < LoadSize; ++i)
	{
		CGameObject* pGameObject = nullptr;

		CEnvironment_Object::ENVIRONMENT_DESC Desc;

		Desc.iLevelIndex = LEVEL_GAMEPLAY;
		Desc.strModelTag = ConvertStrToWstr(LoadJson[i]["ObjectTag"]);
		wstring LayerTag = ConvertStrToWstr(LoadJson[i]["LayerTag"]);

		const json& TransformJson = LoadJson[i]["Component"]["Transform"];

		_float4x4 WorldMatrix;

		for (_int i = 0; i < 4; ++i)
		{
			for (_int j = 0; j < 4; ++j)
			{
				WorldMatrix.m[i][j] = TransformJson[i][j];
			}
		}

		XMStoreFloat4(&Desc.vPos, XMLoadFloat4x4(&WorldMatrix).r[3]);
		Desc.WorldMatrix = WorldMatrix;


		wstring wstr = TEXT("Prototype_GameObject_Environment");
		if (Desc.strModelTag == TEXT("Prototype_Component_Model_Environment_BornFire"))
		{
			wstr = TEXT("Prototype_GameObject_Environment_BornFire");
		}

		if (Desc.strModelTag == TEXT("Prototype_Component_Model_Environment_Ground"))
		{
			Desc.isGroundMesh = true;
		}

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, LayerTag, wstr, &Desc, &pGameObject)))
			return E_FAIL;

		

		//pGameObject->Get_Transform()->Set_WorldFloat4x4(WorldMatrix);
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Collider()
{
	if(FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Player"), TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Monster"), TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Monster"), TEXT("Layer_Monster"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Bullet"), TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Boss"), TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Check_CollisionGroup(TEXT("Layer_Player"), TEXT("Layer_Boss"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Event()
{
	CMyEvent::EVENT_DESC EventDesc;

	EventDesc.eEventType = CMyEvent::BOSS1_APPEAR;
	EventDesc.iCurrentLevelIndex = LEVEL_GAMEPLAY;
	EventDesc.vEventPos = _float3(59.662f, 19.222f, 237.943f);
	EventDesc.vColliderSize = _float3(7.f, 5.f, 2.f);
	EventDesc.vColliderCenter = _float3(0.f, 5.f, 0.f);
	
	CEvent_Boss1Appear* pEvent_Boss1Appear = CEvent_Boss1Appear::Create(m_pDevice, m_pContext, &EventDesc);

	if(nullptr == pEvent_Boss1Appear)
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Event(TEXT("Event_Boss1Appeal"), pEvent_Boss1Appear, &EventDesc)))
		return E_FAIL;

	//CMyEvent* pMyEvent = 

	//if(FAILED(m_pGameInstance->Add_Event(TEXT("Event_BossAppeal"), &EventDesc)))
	//	return E_FAIL;
	
	return S_OK;
}

wstring CLevel_GamePlay::ConvertStrToWstr(const string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
	return wstr;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
