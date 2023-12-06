#include "stdafx.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"

#include "Imgui_Manager.h"

#include "GameInstance.h"
#include "Dynamic_Terrain.h"
#include "TestTree.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

ImGuiIO g_io;

CImgui_Manager::CImgui_Manager()
{
}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = CGameInstance::GetInstance();
	
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	g_io = ImGui::GetIO(); (void)g_io;
	g_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if(!ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if(!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

	ImGui::StyleColorsDark();
	g_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, g_io.Fonts->GetGlyphRangesKorean());

	m_eLevelID = LEVEL_TOOL;

	

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor = ImVec4(0.1, 0.1, 0.1, 0.5);
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	
	_uint WinCX, WinCY;
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;

	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);
	

	ImGui::Begin(u8"메인 툴", &m_bMainTool, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginMenu(u8"툴"))
	{
		ImGui::MenuItem(u8"맵툴", NULL, &m_bMapTool);
		ImGui::MenuItem(u8"이펙트툴", NULL, &m_bEffectTool);
		ImGui::MenuItem(u8"오브젝트툴", NULL, &m_bObjectTool);
		ImGui::MenuItem(u8"카메라툴", NULL, &m_bCameraTool);

		ImGui::EndMenu();
	}

	if (m_bMapTool)	ShowMapTool();
	if (m_bEffectTool) ShowEffectTool();
	if (m_bCameraTool) ShowCameraTool();
	if (m_bObjectTool) ShowObjectTool();

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

	const float clear_color[4] = { 1.f, 1.f, 1.f, 1.f};


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	

	//m_pSwapChain->Present(1, 0);
}

HRESULT CImgui_Manager::Save_EditTexture()
{
	/* 텍스쳐를 생성해보자. */
	

	D3D11_TEXTURE2D_DESC	TextureDesc = {};

	TextureDesc.Width = 1024;
	TextureDesc.Height = 1024;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		InitialData = {};

	_uint* pPixels = new _uint[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}


	InitialData.pSysMem = pPixels;
	InitialData.SysMemPitch = TextureDesc.Width * 4;


	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &m_pTexture2D)))
		return E_FAIL;

	/*pPixels[0] = D3DCOLOR_ARGB(255, 255, 0, 0);*/

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			if (j < TextureDesc.Width * 0.5f)
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
			else
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	/* 텍스쳐의 픽셀정보를 내 마음대로 조절해서 */
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource = {};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixels, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(m_pTexture2D, 0);


	/* 다시 파일로 저장하기위해서. */
	if (FAILED(SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixels);

	return S_OK;
}

char* CImgui_Manager::ConvertWCtoC(const wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;
	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];
	//형 변환
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

wchar_t* CImgui_Manager::ConvertCtoWC(const char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);
	return pStr;
}



void CImgui_Manager::ShowMapTool()
{
	//!Com_VIBuffer
	//! 
	ImGui::Begin(u8"맵툴");
	if (ImGui::BeginTabBar("##MapTabBar"))
	{
		//TODO 타일 탭 시작
		if (ImGui::BeginTabItem(u8"타일"))
		{
			
				ImGui::Text(u8"타일");

				ImGui::InputFloat(u8"입력 X : ", &m_fTileX);
				ImGui::InputFloat(u8"입력 Z : ", &m_fTileZ);

				m_tMapInfo.vPosition.x = m_fTileX;
				m_tMapInfo.vPosition.y = 1.f;
				m_tMapInfo.vPosition.z = m_fTileZ;

				if (ImGui::Button("Create"))
				{
					if (nullptr != m_pDynamic_Terrain)
					{
						m_pDynamic_Terrain->Delete_Component(TEXT("Com_VIBuffer"));
					}

					

					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &m_tMapInfo, reinterpret_cast<CGameObject**>(&m_pDynamic_Terrain))))
						return;

					

					//Safe_AddRef(m_pDynamic_Terrain);
				}
			
				if(m_pDynamic_Terrain)
				ImGui::Checkbox(u8"픽킹", &m_bTileing);

				if (m_bTileing)
				{
					ImGui::Text(u8"Mouse X : %f", m_pDynamic_Terrain->GetMousePos().x);
					ImGui::Text(u8"Mouse Y : %f", m_pDynamic_Terrain->GetMousePos().y);
					ImGui::Text(u8"Mouse Z : %f", m_pDynamic_Terrain->GetMousePos().z);
					

					static float begin = 1, end = 10;

					if(ImGui::DragInt(u8"브러쉬 범위",&m_iBrushRange, 1, 1, 10))
						m_pDynamic_Terrain->SetRadious(m_iBrushRange);

					if (ImGui::DragInt(u8"높이 힘", &m_iBrushPower, 1, 1, 10))
						m_pDynamic_Terrain->SetPower(m_iBrushPower);



					static int iMode = 0;

					ImGui::RadioButton(u8"HEIGHT_FLAT", &iMode, 0);
					ImGui::RadioButton(u8"HEIGHT_LERP", &iMode, 1);
					ImGui::RadioButton(u8"HEIGHT_SET", &iMode, 2);
					ImGui::RadioButton(u8"FILLTER", &iMode, 3);

					
					m_pDynamic_Terrain->Picking_Terrain((CDynamic_Terrain::EDIT_MODE)iMode);
				

				
			}
			ImGui::EndTabItem();
		}
		//TODO 타일 탭 종료

		//! 환경 탭 시작
		if (ImGui::BeginTabItem(u8"환경"))
		{
			ImGui::Text(u8"환경");
			
			if(m_pDynamic_Terrain)
				ImGui::Text(u8"Mouse X : %f", m_pDynamic_Terrain->GetTerrainPos().x);
				ImGui::Text(u8"Mouse Y : %f", m_pDynamic_Terrain->GetTerrainPos().y);
				ImGui::Text(u8"Mouse Z : %f", m_pDynamic_Terrain->GetTerrainPos().z);

				//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &m_tMapInfo, reinterpret_cast<CGameObject**>(&m_pDynamic_Terrain))))
				//	return;

			ImGui::Checkbox(u8"픽킹", &m_bPicking);

			if (m_bPicking)
			{
				static int iMode = 0;

				//ImGui::RadioButton(u8"HEIGHT_FLAT", &iMode, 0);
				//ImGui::RadioButton(u8"HEIGHT_LERP", &iMode, 1);
				//ImGui::RadioButton(u8"HEIGHT_SET", &iMode, 2);
				//ImGui::RadioButton(u8"FILLTER", &iMode, 3);

				if (m_pGameInstance->Mouse_Down(DIM_LB))
				{
					//typedef struct tagTreeDesc : public CGameObject::GAMEOBJECT_DESC
					//{
					//	_float4 vPos = { 0.f, 0.f, 0.f, 0.f };
					//}TREE_DESC;

					CTestTree::TREE_DESC pDesc = {};
					m_fPickingPos = m_pDynamic_Terrain->GetTerrainPos();

					_float4 fResultPos = { m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1};
					pDesc.vPos = fResultPos;
					

					CTestTree* pTree = nullptr;

					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ForkLift"), &pDesc, reinterpret_cast<CGameObject**>(&pTree))))
							return;

					m_vecObject.push_back(pTree);

				}
					//CTestTree::Clone()



			}

			ImGui::EndTabItem();
		}
		//! 환경 탭 종료

		//! 높이 탭 시작
		if (ImGui::BeginTabItem(u8"높이"))
		{
			ImGui::Text(u8"높이");
			ImGui::EndTabItem();
		}
		//! 높이 탭 종료

		ImGui::EndTabBar();
	}
	ImGui::End();
}



void CImgui_Manager::ShowEffectTool()
{
	ImGui::Begin(u8"이펙트툴");
	if (ImGui::BeginTabBar("##EffectTabBar"))
	{
		//TODO 이펙트 탭 시작
		if (ImGui::BeginTabItem(u8"이펙트"))
		{
			ImGui::Text(u8"이펙트");
			ImGui::EndTabItem();
		}
		//TODO 이펙트 탭 종료

		//! 파티클 탭 시작
		if (ImGui::BeginTabItem(u8"파티클"))
		{
			ImGui::Text(u8"파티클");
			ImGui::EndTabItem();
		}
		//! 파티클 탭 종료
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowObjectTool()
{
	ImGui::Begin(u8"오브젝트툴");
	if (ImGui::BeginTabBar("##ObjectTabBar"))
	{
		//TODO 오브젝트1 탭 시작
		if (ImGui::BeginTabItem(u8"오브젝트1"))
		{
			ImGui::Text(u8"오브젝트1");
			ImGui::EndTabItem();
		}
		//TODO 오브젝트1 탭 종료

		//! 오브젝트2 탭 시작
		if (ImGui::BeginTabItem(u8"오브젝트2"))
		{
			ImGui::Text(u8"오브젝트2");
			ImGui::EndTabItem();
		}
		//! 오브젝트2 탭 종료
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowCameraTool()
{
	ImGui::Begin(u8"카메라툴");
	if (ImGui::BeginTabBar("##CameraTabBar"))
	{
		//TODO 베지어 탭 시작
		if (ImGui::BeginTabItem(u8"베지어"))
		{
			ImGui::Text(u8"베지어");
			ImGui::EndTabItem();
		}
		//TODO 베지어 탭 종료

		//! 메뉴얼 탭 시작
		if (ImGui::BeginTabItem(u8"메뉴얼"))
		{
			ImGui::Text(u8"메뉴얼");
			ImGui::EndTabItem();
		}
		//! 메뉴얼 탭 종료
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::Picking(TOOLID eToolID, _int iMode)
{
	
	_uint WinCX, WinCY;
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;			RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);

	m_pDynamic_Terrain->Get_WorldMatrix();
	

	switch (eToolID)
	{
		case Client::CImgui_Manager::TOOL_MAP:
			m_pDynamic_Terrain->Picking_Terrain((CDynamic_Terrain::EDIT_MODE)iMode);
			break;
		case Client::CImgui_Manager::TOOL_OBJECT:
			break;
		case Client::CImgui_Manager::TOOL_CAMERA:
			break;
		case Client::CImgui_Manager::TOOL_EFFECT:
			break;
	}
}



void CImgui_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	//Safe_Release(m_pDynamic_Terrain);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImgui_Manager::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(desc);
		ImGui::EndTooltip();
	}
}

char* CImgui_Manager::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* result = new char[size_needed];

	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);

	return result;
}