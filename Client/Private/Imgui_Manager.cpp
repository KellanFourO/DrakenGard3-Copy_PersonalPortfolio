#include "stdafx.h"
#include "../Default/stdafx.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"


IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

CImgui_Manager::~CImgui_Manager()
{
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
}

HRESULT CImgui_Manager::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	
	Safe_AddRef(m_pGameInstance);

	if (!CreateDeviceD3D())
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigViewportsNoAutoMerge = true;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	if(!ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if(!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

	ImGui::StyleColorsDark();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

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
	
	ImGui::Begin(u8"���� ��", &m_bMainTool, ImGuiWindowFlags_AlwaysAutoResize);

// 	_bool bDemo;
// 	ImGui::ShowDemoWindow(&bDemo);
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
	if (m_bReady)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m_bReady = false;
	}

	ImGui::Render();

	const float clear_color[4] = { 1.f, 1.f, 1.f, 1.f};

	m_pContext->OMSetRenderTargets(1, &m_pMainRenderTargetView, NULL);
	m_pContext->ClearRenderTargetView(m_pMainRenderTargetView, clear_color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//m_pSwapChain->Present(1, 0);
}

HRESULT CImgui_Manager::Save_EditTexture()
{
	/* �ؽ��ĸ� �����غ���. */
	

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

	/* �ؽ����� �ȼ������� �� ������� �����ؼ� */
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource = {};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixels, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(m_pTexture2D, 0);


	/* �ٽ� ���Ϸ� �����ϱ����ؼ�. */
	if (FAILED(SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"))))
		return E_FAIL;

	return S_OK;
}

_bool CImgui_Manager::CreateDeviceD3D()
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, &featureLevel, &m_pContext) != S_OK)
		return false;
	
	CreateRenderTarget();

	return true;
}

void CImgui_Manager::CleanupDeviceD3D()
{
	CleanUpRenderTarget();
	if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = NULL; }
	if (m_pContext) { m_pContext->Release(); m_pContext = NULL; }
	if (m_pDevice) { m_pDevice->Release(); m_pDevice = NULL; }
}

void CImgui_Manager::ResizeImGui(WPARAM wParam, LPARAM lParam)
{
	m_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
}

void CImgui_Manager::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);
	pBackBuffer->Release();
}

void CImgui_Manager::CleanUpRenderTarget()
{
	if (m_pMainRenderTargetView) { m_pMainRenderTargetView->Release(); m_pMainRenderTargetView = NULL; }
}

char* CImgui_Manager::ConvertWCtoC(const wchar_t* str)
{
	//��ȯ�� char* ���� ����
	char* pStr;
	//�Է¹��� wchar_t ������ ���̸� ����
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* �޸� �Ҵ�
	pStr = new char[strSize];
	//�� ��ȯ
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

wchar_t* CImgui_Manager::ConvertCtoWC(const char* str)
{
	//wchar_t�� ���� ����
	wchar_t* pStr;
	//��Ƽ ����Ʈ ũ�� ��� ���� ��ȯ
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t �޸� �Ҵ�
	pStr = new WCHAR[strSize];
	//�� ��ȯ
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);
	return pStr;
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

		//! ���� �� ����
		if (ImGui::BeginTabItem(u8"����"))
		{
			ImGui::Text(u8"����");
			ImGui::EndTabItem();
		}
		//! ���� �� ����

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