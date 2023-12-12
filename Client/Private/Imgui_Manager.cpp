#pragma once
#include "stdafx.h"

#include "Tool_Define.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Dynamic_Terrain.h"
#include "Camera_MapTool.h"

#include <regex>
#include <codecvt>

ImGuiIO g_io;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE	   mCurrentGizmoMode(ImGuizmo::WORLD);
static bool useSnap(false);

IMPLEMENT_SINGLETON(CImgui_Manager);

CImgui_Manager::CImgui_Manager()
	: m_bObjectTool(false)
	, m_bCameraTool(false)
	, m_bEffectTool(false)
	, m_bMapTool(false)
	, m_bReady(true)
{
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_eLevelID = LEVEL_TOOL;

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	if (FAILED(ImGui_Initialize()))	//! ImGui 초기화
		return E_FAIL;

	ImGuizmo_Initialize();			//! ImGui 즈모 초기화
	ImGuiFileDialog_Intialize();	//! Imgui 파일다이얼로그 초기화

	Ready_ProtoTagList();			//! 오브젝트 프로토태그 리스트 준비

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	//! 레이 월드좌표 갱신
	UpdateRay();

	ImGui_BeginTick();

	//#Imgui_MainTool_Start
	ImGui::Begin(u8"메인 툴");
	
	ImGui_MapToolTick();	
	
	//#Imgui_MainTool_End
	ImGui::End(); 

}

void CImgui_Manager::Render()
{
	if (m_bReady)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m_bReady = false;
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

HRESULT CImgui_Manager::ImGui_Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	g_io = ImGui::GetIO(); (void)g_io;
	g_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (!ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if (!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

	ImGui::StyleColorsDark();
	g_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, g_io.Fonts->GetGlyphRangesKorean());
}

void CImgui_Manager::ImGuizmo_Initialize()
{
	//! 기즈모 뷰,투영 멤버변수 메모리 할당
	m_arrView = new _float[16];
	m_arrProj = new _float[16];
	//! 기즈모 뷰,투영 멤버변수 메모리 할당 끝
}

void CImgui_Manager::ImGuiFileDialog_Intialize()
{
	m_pFileDialogOpen = ImGuiFileDialog::Instance();
	m_pFileDialogOpen = ImGuiFileDialog::Instance();

	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "((Custom.+[.]h))", ImVec4(0.1f, 0.9f, 0.1f, 0.9f));  // use a regex
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);  // add an icon for the filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");             // add an text for a filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);     // for all dirs
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, nullptr, ImVec4(0.2f, 0.9f, 0.2f, 0.9f), ICON_IGFD_FILE);                              // for all link files
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByTypeLink, nullptr, ImVec4(0.8f, 0.8f, 0.8f, 0.8f), ICON_IGFD_FOLDER);  // for all link dirs
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByTypeLink, nullptr, ImVec4(0.8f, 0.8f, 0.8f, 0.8f), ICON_IGFD_FILE);   // for all link files
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

	ImGuiFileDialog::Instance()->AddBookmark("Bin", "../Bin/");
	ImGuiFileDialog::Instance()->AddBookmark("MapData", "../Bin/DataFiles/Map/");
	ImGuiFileDialog::Instance()->AddBookmark("ObjectData", "../Bin/DataFiles/Object/");
}

void CImgui_Manager::ImGui_BeginTick()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
	
	
}

void CImgui_Manager::ImGui_MapToolTick()
{
	

	if (ImGui::BeginTabBar(u8"맵"))
	{
		m_eToolID = CImgui_Manager::TOOL_MAP;

		if (ImGui::BeginTabItem(u8"맵"))
		{
			if (ImGui::Button(u8"저장하기"))	{ /*TODO 저장 함수 호출하기 */ } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { /*TODO 불러오기 함수 호출하기 */ }

			ImGui::InputFloat(u8"X 사이즈", &m_fTileX);
			ImGui::InputFloat(u8"Z 사이즈", &m_fTileZ);

			m_tMapInfo.vPosition.x = m_fTileX;
			m_tMapInfo.vPosition.y = 1.f;
			m_tMapInfo.vPosition.z = m_fTileZ;

			if (ImGui::Button(u8"생성"))
			{
				if (nullptr != m_pDynamic_Terrain)
				{
					m_pDynamic_Terrain->Delete_Component(TEXT("Com_VIBuffer"));
				}
				else
					MSG_BOX("Dynamic Terrain is Nullptr");
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void CImgui_Manager::UpdateRay()
{
	_uint WinCX = g_iWinSizeX;
	_uint WinCY = g_iWinSizeY;

	m_tWorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);
}

HRESULT CImgui_Manager::Ready_ProtoTagList()
{
	return S_OK;
}


void CImgui_Manager::Free()
{
}

