#include "stdafx.h"
#include <regex>
#include <codecvt>

#include "Tool_Define.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Dynamic_Terrain.h"
#include "Camera_MapTool.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

ImGuiIO g_io;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE	   mCurrentGizmoMode(ImGuizmo::WORLD);
static bool useSnap(false);

CImgui_Manager::CImgui_Manager()
{
}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_bObjectTool = false;
	m_bCameraTool = false;
	m_bEffectTool = false;
	m_bMapTool = false;

	//!Guizmo 뷰,투영 할당용 변수
	m_arrView = new float[16];
	m_arrProj = new float[16];

	m_pGameInstance = CGameInstance::GetInstance();

	m_pFileDialogOpen = ImGuiFileDialog::Instance();
	m_pFileDialogExport = ImGuiFileDialog::Instance();
	IntializeColor();


	Safe_AddRef(m_pGameInstance);

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

	m_eLevelID = LEVEL_TOOL;


	Ready_ProtoTagList();

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	BeginFrame();

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

	ShowDialog(m_strCurrentDialogTag);

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

	const float clear_color[4] = { 1.f, 1.f, 1.f, 1.f };


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



void CImgui_Manager::KeyInput()
{
	switch (m_eToolID)
	{
	case Client::CImgui_Manager::TOOL_MAP:
		MapToolKeyInput();
		break;
	case Client::CImgui_Manager::TOOL_OBJECT:
		break;
	case Client::CImgui_Manager::TOOL_CAMERA:
		break;
	case Client::CImgui_Manager::TOOL_EFFECT:
		break;

	}

}

void CImgui_Manager::MapToolKeyInput()
{
	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_bPicking = !m_bPicking;
	}
}

HRESULT CImgui_Manager::Add_PrototypeTag(const wstring& strPrototypeTag)
{
	string pTag = ConverWstrToStr(strPrototypeTag);

	m_vecObjectProtoTags.push_back(pTag);

	return S_OK;
}

HRESULT CImgui_Manager::Ready_ProtoTagList()
{
	vector<wstring> vecTags = m_pGameInstance->Get_VecTags();

	for (auto& strTag : vecTags)
	{
		Add_PrototypeTag(strTag);
	}

	return S_OK;
}

void CImgui_Manager::ObjectToolKeyInput()
{
	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_bPicking = !m_bPicking;
	}

	if (m_PickingObject && m_pGameInstance->Key_Pressing(DIK_T))
	{
		m_PickingObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1.f));
	}
}

string CImgui_Manager::SliceObjectTag(string strFullTag)
{
	regex pattern("Prototype_GameObject_(\\w+)");

	smatch match;

	if (regex_search(strFullTag, match, pattern))
	{
		return match[1].str();
	}

	return string();

	//m_pGameObject = pObj;
	//wstring str = (*pObj).Get_Name();
	//size_t pos = str.rfind(L"_");
	//if (pos != wstring::npos)
	//{
	//	wstring tag = str.substr(pos + 1);
	//	m_strPickModelTag = ConverWstrToStr(tag);
	//}
}

void CImgui_Manager::CreateGuizmo()
{
	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	_float* arrView = m_arrView;
	_float* arrProj = m_arrProj;

	_float4x4 matWorld = m_PickingObject->Get_Transform()->Get_WorldFloat4x4();
	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGui::DragFloat3("Rt", matrixRotation);
	ImGui::DragFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);


	ImGui::Checkbox("UseSnap", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::DragFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::DragFloat3("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::DragFloat3("Scale Snap", &snap[0]);
		break;
	}

	ImGuizmo::Manipulate(arrView, arrProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	_float4x4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
					arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
					arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
					arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };

	m_PickingObject->Get_Transform()->Set_WorldFloat4x4(matW);


	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}

}

void CImgui_Manager::Set_View()
{
	_float4x4 matCamView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float	  arrView[] = { matCamView._11,matCamView._12,matCamView._13,matCamView._14,
						  matCamView._21,matCamView._22,matCamView._23,matCamView._24,
						  matCamView._31,matCamView._32,matCamView._33,matCamView._34,
						  matCamView._41,matCamView._42,matCamView._43,matCamView._44 };

	memcpy(m_arrView, &arrView, sizeof(arrView));

}

void CImgui_Manager::Set_Proj()
{
	_float4x4 matCamProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	_float	  arrProj[] = { matCamProj._11,matCamProj._12,matCamProj._13,matCamProj._14,
						  matCamProj._21,matCamProj._22,matCamProj._23,matCamProj._24,
						  matCamProj._31,matCamProj._32,matCamProj._33,matCamProj._34,
						  matCamProj._41,matCamProj._42,matCamProj._43,matCamProj._44 };
	memcpy(m_arrProj, &arrProj, sizeof(arrProj));
}

_bool CImgui_Manager::Check_ImGui_Rect()
{
	POINT tMouse;

	GetCursorPos(&tMouse);
	ScreenToClient(m_pGameInstance->Get_GraphicDesc().hWnd, &tMouse);


	ImVec2 windowPos = ImGui::GetWindowPos(); //왼쪽상단모서리점
	ImVec2 windowSize = ImGui::GetWindowSize();

	if (tMouse.x >= windowPos.x && tMouse.x <= windowPos.x + windowSize.x &&
		tMouse.y >= windowPos.y && tMouse.y <= windowPos.y + windowSize.y)
	{
		return false; //ImGui 영역 내
	}

	return true;
}

void CImgui_Manager::ImGui_Initialize()
{
}

string CImgui_Manager::ConverWstrToStr(const wstring& str)
{
	wstring_convert<codecvt_utf8<_tchar>> converter;
	string ChangeStr = converter.to_bytes(str);
	return ChangeStr;
}

wstring CImgui_Manager::ConverStrToWstr(const string& str)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	wstring wideStr = converter.from_bytes(str);
	return wideStr;
}

void CImgui_Manager::SaveDialog(TOOLID eToolID)
{
	if (!m_pFileDialogExport->IsOpened())
	{
		if (m_pFileDialogOpen->IsOpened())
			m_pFileDialogOpen->Close();

		m_strCurrentDialogTag = "MapToolSave";
		m_eDialogMode = CImgui_Manager::DIALOG_SAVE;
		m_pFileDialogExport->OpenDialog("MapToolSave", u8"맵 데이터 저장", ".json", ".", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);

		m_bDialog = true;
	}
}

void CImgui_Manager::LoadDialog(TOOLID eToolID)
{
	if (!m_pFileDialogOpen->IsOpened())
	{
		if (m_pFileDialogExport->IsOpened())
			m_pFileDialogExport->Close();
		m_strCurrentDialogTag = "MapToolLoad";
		m_eDialogMode = CImgui_Manager::DIALOG_LOAD;
		m_pFileDialogOpen->OpenDialog("MapToolLoad", u8"맵 데이터 불러오기", ".json", ".", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ReadOnlyFileNameField | ImGuiFileDialogFlags_HideColumnType);
		m_bDialog = true;
	}

}

void CImgui_Manager::IntializeColor()
{
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

void CImgui_Manager::ShowDialog(string& strDialogTag)
{
	ImGuiFileDialog* pDialog = ImGuiFileDialog::Instance();

	switch (m_eDialogMode)
	{
	case Client::CImgui_Manager::DIALOG_SAVE:

		if (pDialog->Display(strDialogTag))
		{
			if (pDialog->IsOk())
			{
				string filePathName = pDialog->GetFilePathName();
				string filePath = pDialog->GetCurrentPath();
				string fileName = pDialog->GetCurrentFileName();

				string userDatas;
				if (pDialog->GetUserDatas())
					userDatas = std::string((const char*)pDialog->GetUserDatas());
				auto selection = pDialog->GetSelection(); // multiselection

				json OutJson;

				m_pDynamic_Terrain->Write_Json(OutJson);

				if (FAILED(CJson_Utility::Save_Json(filePathName.c_str(), OutJson)))
				{
					MSG_BOX("맵 세이브 실패");
				}
				else
				{
					MSG_BOX("저장 성공");
				}

			}

			pDialog->Close();
		}

		break;

	case Client::CImgui_Manager::DIALOG_LOAD:

		if (pDialog->Display(strDialogTag))
		{
			if (pDialog->IsOk())
			{
				string filePathName = pDialog->GetFilePathName();
				string filePath = pDialog->GetCurrentPath();

				string userDatas;
				if (pDialog->GetUserDatas())
					userDatas = std::string((const char*)pDialog->GetUserDatas());
				auto selection = pDialog->GetSelection(); // multiselection

				json	InJson;

				CJson_Utility::Load_Json(filePathName.c_str(), InJson);

				if (m_pDynamic_Terrain)
					m_pDynamic_Terrain->Load_FromJson(InJson);

				else
				{
					//Out_Json.emplace("SizeX", m_tDynamicInfo.fX);
					//Out_Json.emplace("SizeY", m_tDynamicInfo.fY);
					//Out_Json.emplace("SizeZ", m_tDynamicInfo.fZ);

					CDynamic_Terrain::DINFO tInfo;
					tInfo.fX = InJson["SizeX"];
					tInfo.fY = InJson["SizeY"];
					tInfo.fZ = InJson["SizeZ"];

					m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &tInfo);
				}
			}

			pDialog->Close();
		}
		break;
	}

}

void CImgui_Manager::ShowMapTool()
{
	MapToolKeyInput();

	ImGui::Begin(u8"맵툴");
	if (ImGui::BeginTabBar("##MapTabBar"))
	{

		m_eToolID = CImgui_Manager::TOOL_MAP;

		//TODO 타일 탭 시작
		if (ImGui::BeginTabItem(u8"타일"))
		{
			m_eMapTapID = TAP_TILE;
			ImGui::Text(u8"타일");

			if (ImGui::Button(u8"저장하기"))
			{
				SaveDialog(m_eToolID);
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"불러오기"))
			{
				LoadDialog(m_eToolID);
			}

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
			}

			if (m_pDynamic_Terrain)
				ImGui::Checkbox(u8"픽킹", &m_bTileing);

			if (m_bTileing)
			{
				ImGui::Text(u8"Mouse X : %f", m_pDynamic_Terrain->GetMousePos().x);
				ImGui::Text(u8"Mouse Y : %f", m_pDynamic_Terrain->GetMousePos().y);
				ImGui::Text(u8"Mouse Z : %f", m_pDynamic_Terrain->GetMousePos().z);


				static float begin = 1, end = 10;

				if (ImGui::DragInt(u8"브러쉬 범위", &m_iBrushRange, 1, 1, 10))
					m_pDynamic_Terrain->SetRadious(m_iBrushRange);

				if (ImGui::DragInt(u8"높이 힘", &m_iBrushPower, 1, 1, 10))
					m_pDynamic_Terrain->SetPower(m_iBrushPower);


				ImGui::RadioButton(u8"HEIGHT_FLAT", &m_iTileMode, 0);
				ImGui::RadioButton(u8"HEIGHT_LERP", &m_iTileMode, 1);
				ImGui::RadioButton(u8"HEIGHT_SET", &m_iTileMode, 2);
				ImGui::RadioButton(u8"FILLTER", &m_iTileMode, 3);

				m_pDynamic_Terrain->Picking_Terrain((CDynamic_Terrain::EDIT_MODE)m_iTileMode);
			}
			ImGui::EndTabItem();
		}
		//TODO 타일 탭 종료



		//! 높이 탭 시작
		if (ImGui::BeginTabItem(u8"높이"))
		{
			ImGui::Text(u8"높이");
			ImGui::EndTabItem();
		}
		//! 높이 탭 종료

		m_eMapTapID = TAP_END;
		m_eToolID = TOOL_END;
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

	ObjectToolKeyInput();

	ImGui::Begin(u8"오브젝트툴");
	if (ImGui::BeginTabBar("##ObjectTabBar"))
	{
		//TODO 애니메이션 모델 탭 시작
		if (ImGui::BeginTabItem(u8"애니메이션 모델"))
		{
			if (m_pDynamic_Terrain)
			{
				ImGui::Text(u8"Mouse X : %f", m_pDynamic_Terrain->GetMousePos().x);
				ImGui::Text(u8"Mouse Y : %f", m_pDynamic_Terrain->GetMousePos().y);
				ImGui::Text(u8"Mouse Z : %f", m_pDynamic_Terrain->GetMousePos().z);
			}

			ImGui::Checkbox(u8"픽킹", &m_bPicking);

			if (m_bPicking)
			{
				ImGui::RadioButton(u8"Create", &m_iObjectMode, 0);
				ImGui::RadioButton(u8"Select", &m_iObjectMode, 1);
				//ImGui::RadioButton(u8"HEIGHT_SET", &iMode, 2);
				//ImGui::RadioButton(u8"FILLTER", &iMode, 3);
				if (m_iObjectMode == 0)
				{
					//! 데이터매니저로부터 태그리스트 얻어오는 함수
					_int ObjectTagsSize = m_vecObjectProtoTags.size();

					if (ImGui::BeginListBox(u8"태그 리스트"))
					{
						for (int n = 0; n < ObjectTagsSize; ++n)
						{
							const _bool isSelected = (m_iSelectTagIndex == n);

							if (ImGui::Selectable(m_vecObjectProtoTags[n].c_str(), isSelected))
							{
								m_iSelectTagIndex = n;

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndListBox();
					}


					if (m_pGameInstance->Mouse_Down(DIM_LB))
					{

						if (m_pDynamic_Terrain)
							m_fPickingPos = m_pDynamic_Terrain->GetMousePos();

						CGameObject* pObject = nullptr;

						CGameObject::GAMEOBJECT_DESC pDesc;

						pDesc.vPos = { m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1.f };
						pDesc.isPicking = true;
						pDesc.fRotationPerSec = 1.f;
						pDesc.fSpeedPerSec = 1.f;

						if (m_pDynamic_Terrain && m_pDynamic_Terrain->MouseOnTerrain() && Check_ImGui_Rect())
						{


							wstring wstr = ConverStrToWstr(m_vecObjectProtoTags[m_iSelectTagIndex]);
							if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), wstr, &pDesc, reinterpret_cast<CGameObject**>(&pObject))))
								return;

							m_vecObjects.push_back(pObject);


							_char SliceTag[MAX_PATH];
							string strTemp;

							strTemp = SliceObjectTag(m_vecObjectProtoTags[m_iSelectTagIndex]);
							// pushIndex 값을 문자열로 변환하여 연결
							char pushIndexStr[10];
							sprintf_s(pushIndexStr, sizeof(pushIndexStr), " %d", m_vecCreateObjectTag.size());
							strTemp += pushIndexStr;

							m_vecCreateObjectTag.push_back(strTemp);


							// 									strncpy(SliceTag,strTemp.c_str(), sizeof(SliceTag));
							// 
							// 									char pushIndexStr[10];
							// 									sprintf_s(pushIndexStr, sizeof(pushIndexStr), " %d", pushIndex);
							// 									strcat_s(SliceTag, pushIndexStr);
							// 
							// 									m_vecCreateObjectTag.push_back(SliceTag);
							// 
							// 									pushIndex++;



						}
					}
				}

				if (m_iObjectMode == 1)
				{
					Set_View();
					Set_Proj();

					if (m_PickingObject)
						CreateGuizmo();

					_int iObjectListSize = m_vecCreateObjectTag.size();

					if (ImGui::BeginListBox(u8""))
					{
						for (int n = 0; n < iObjectListSize; ++n)
						{
							const _bool isSelected = (m_iPickingObjectIndex == n);

							if (ImGui::Selectable(m_vecCreateObjectTag[n].c_str(), isSelected))
							{
								m_PickingObject = m_vecObjects[n];
								m_iPickingObjectIndex = n;

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndListBox();
					}

					if (m_pDynamic_Terrain)
						m_fPickingPos = m_pDynamic_Terrain->GetMousePos();

					_uint iWinCX = g_iWinSizeX;
					_uint iWinCY = g_iWinSizeY;

					//! 픽킹된 오브젝트가 있는 경우
						//! T 키를 누르고 있을 때의 따라오게 만들자.

					//! 픽킹된 오브젝트가 없는 경우
						//! P 키를 누르고 있을때 오브젝트들을 순회하면서 픽킹 검사를 하고 픽킹오브젝트로 대입하자.


					_int iObjectSize = m_vecObjects.size();

					if (m_pGameInstance->Key_Pressing(DIK_P))
					{
						m_bPressing = true;

						for (_int i = 0; i < iObjectSize; ++i)
						{
							if (m_vecObjects[i]->Picking(m_fPickingPos))
							{
								m_PickingObject = m_vecObjects[i];
								m_iPickingObjectIndex = i;

							}
						}
					}
					else
						m_bPressing = false;
				}
			}

			ImGui::EndTabItem();
		}
		//TODO 오브젝트1 탭 종료

		//! 논애님 모델 탭 시작
		if (ImGui::BeginTabItem(u8"논애님 모델"))
		{
			ImGui::EndTabItem();
		}
		//! 논애님 모델 탭 종료
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowCameraTool()
{
	ImGui::Begin(u8"카메라툴");
	if (ImGui::BeginTabBar("##CameraTabBar"))
	{
		//TODO 스플라인 탭 시작
		if (ImGui::BeginTabItem(u8"스플라인"))
		{
			ImGui::Text(u8"스플라인");


			if (ImGui::BeginListBox(u8"리스트박스"))
			{

				ImGui::EndListBox();
			}

			ImGui::EndTabItem();
		}
		//TODO 스플라인 탭 종료

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
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;

	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);

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


	m_vecObjectProtoTags.clear();

	m_vecCreateObjectTag.clear();
	//Safe_Release(m_pDynamic_Terrain);

	Safe_Delete_Array(m_arrView);
	Safe_Delete_Array(m_arrProj);

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