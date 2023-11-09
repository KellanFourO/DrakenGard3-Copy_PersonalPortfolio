#include "stdafx.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Imgui_Manager.h"

typedef struct ImGui_ImplDX11_ViewportData
{
	IDXGISwapChain* SwapChain;
	ID3D11RenderTargetView* RTView;

	ImGui_ImplDX11_ViewportData() { SwapChain = nullptr; RTView = nullptr; }
	~ImGui_ImplDX11_ViewportData() { IM_ASSERT(SwapChain == nullptr && RTView == nullptr); }
}USERDATA;

ImGuiIO* m_pIo = { nullptr };

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CImgui_Manager::SetUp_Imgui()
{
	USERDATA Test;

	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	//! �ؽ���(�����)�� �����ϴ� ����
	
	
	

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_pIo = &ImGui::GetIO(); (void)m_pIo;
	m_pIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_pIo->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	m_pIo->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	m_pIo->BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	m_pIo->BackendRendererUserData = nullptr;
	m_pIo->ConfigViewportsNoAutoMerge = true;
	//const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	
	//ID3D11RenderTargetView* g_mainRenderTargetView = CGameInstance::GetInstance()->Get_Graphic_Dev()->Get_RTV();
	//m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);

	ImGui::StyleColorsDark();
	m_pIo->Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, m_pIo->Fonts->GetGlyphRangesKorean());

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	//bool show_demo_window = true;
	//bool show_another_window = false;
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	

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

	ImGui::SetNextWindowSize(ImVec2(g_iWinSizeX, g_iWinSizeY), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	
	//	ImGui::SetNextWindowBgAlpha(0.1f); 
	
	ImGui::Begin(u8"���� ��", &m_bMainTool, ImGuiWindowFlags_AlwaysAutoResize);


	if (ImGui::BeginMenu(u8"��"))
	{
		ImGui::MenuItem(u8"����", NULL, &m_bMapTool);
		ImGui::MenuItem(u8"����Ʈ��", NULL, &m_bEffectTool);
		ImGui::MenuItem(u8"������Ʈ��", NULL, &m_bObjectTool);
		ImGui::MenuItem(u8"ī�޶���", NULL, &m_bCameraTool);

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
	ImGui::EndFrame();
	ImGui::Render();

	
	//ID3D11RenderTargetView* g_mainRenderTargetView = CGameInstance::GetInstance()->Get_Graphic_Dev()->Get_RTV();
	//m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (m_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

	if (FAILED(pInstance->SetUp_Imgui()))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Manager::Free()
{
	__super::Free();

	
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}



void CImgui_Manager::ShowMapTool()
{
	ImGui::Begin(u8"����");
	if (ImGui::BeginTabBar("##MapTabBar"))
	{
		//TODO Ÿ�� �� ����
		if (ImGui::BeginTabItem(u8"Ÿ��"))
		{
			ImGui::Text(u8"Ÿ��");
			ImGui::EndTabItem();
		}
		//TODO Ÿ�� �� ����

		//! ȯ�� �� ����
		if (ImGui::BeginTabItem(u8"ȯ��"))
		{
			ImGui::Text(u8"ȯ��");
			ImGui::EndTabItem();
		}
		//! ȯ�� �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
}



void CImgui_Manager::ShowEffectTool()
{
	ImGui::Begin(u8"����Ʈ��");
	if (ImGui::BeginTabBar("##EffectTabBar"))
	{
		//TODO ����Ʈ �� ����
		if (ImGui::BeginTabItem(u8"����Ʈ"))
		{
			ImGui::Text(u8"����Ʈ");
			ImGui::EndTabItem();
		}
		//TODO ����Ʈ �� ����

		//! ��ƼŬ �� ����
		if (ImGui::BeginTabItem(u8"��ƼŬ"))
		{
			ImGui::Text(u8"��ƼŬ");
			ImGui::EndTabItem();
		}
		//! ��ƼŬ �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowObjectTool()
{
	ImGui::Begin(u8"������Ʈ��");
	if (ImGui::BeginTabBar("##ObjectTabBar"))
	{
		//TODO ������Ʈ1 �� ����
		if (ImGui::BeginTabItem(u8"������Ʈ1"))
		{
			ImGui::Text(u8"������Ʈ1");
			ImGui::EndTabItem();
		}
		//TODO ������Ʈ1 �� ����

		//! ������Ʈ2 �� ����
		if (ImGui::BeginTabItem(u8"������Ʈ2"))
		{
			ImGui::Text(u8"������Ʈ2");
			ImGui::EndTabItem();
		}
		//! ������Ʈ2 �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowCameraTool()
{
	ImGui::Begin(u8"ī�޶���");
	if (ImGui::BeginTabBar("##CameraTabBar"))
	{
		//TODO ������ �� ����
		if (ImGui::BeginTabItem(u8"������"))
		{
			ImGui::Text(u8"������");
			ImGui::EndTabItem();
		}
		//TODO ������ �� ����

		//! �޴��� �� ����
		if (ImGui::BeginTabItem(u8"�޴���"))
		{
			ImGui::Text(u8"�޴���");
			ImGui::EndTabItem();
		}
		//! �޴��� �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
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