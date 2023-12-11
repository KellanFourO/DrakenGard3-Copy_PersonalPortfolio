#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

public:
	enum TOOLID { TOOL_MAP, TOOL_OBJECT, TOOL_CAMERA, TOOL_EFFECT, TOOL_END };
	enum MAPTAPID { TAP_TILE, TAP_ENVIRONMENT, TAP_END };
	enum DIALOGID { DIALOG_SAVE, DIALOG_LOAD, DIALOG_END };

private:
			 CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void		Tick(_float fTimeDelta);
	void		Render();

	LEVEL		  Get_Level_ID() { return m_eLevelID; }
	void		  Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }
	class CCamera_MapTool**		Get_Cam() { return &m_pCamera; }

	void					KeyInput();
	_bool					Check_ImGui_Rect();

//TODO 맵 툴 함수 시작

public:
	HRESULT	Save_EditTexture();
	void	MapToolKeyInput();
	void	Set_ToolTerrain(class CDynamic_Terrain* pTerrain) { m_pDynamic_Terrain = pTerrain;}
//TODO 맵 툴 함수 종료

//TODO 오브젝트 툴 함수 시작

private:
	HRESULT Add_PrototypeTag(const wstring& strPrototypeTag);
	HRESULT	Ready_ProtoTagList();
	void	ObjectToolKeyInput();
	string	SliceObjectTag(string strFullTag);
	void	CreateGuizmo();
	void	Set_View();
	void	Set_Proj();

//TODO 오브젝트 툴 함수 종료


private:
	ID3D11Texture2D*	m_pTexture2D = { nullptr };
	string		ConverWstrToStr(const wstring& str);
	wstring		ConverStrToWstr(const string& str);

	//!Imgui Dialog
	void		SaveDialog(TOOLID eToolID);
	void		LoadDialog(TOOLID eToolID);
	void		IntializeColor();
	void		ShowDialog(string& strDialogTag);
	
	

private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CDynamic_Terrain*		m_pDynamic_Terrain = { nullptr };
	class CCamera_MapTool*		m_pCamera = { nullptr };
	

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_bReady = true;
	_bool					m_bMainTool = { true };
	_bool					m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };
	_bool					m_bDialog = false;

	LEVEL					m_eLevelID = { LEVEL_END };
	TOOLID					m_eToolID = { TOOL_END };
	RAY						m_tRay;

	//!ImGui Dialog
	ImGuiFileDialog*	m_pFileDialogOpen;
	ImGuiFileDialog*	m_pFileDialogExport;
	string				m_strCurrentDialogTag;
	DIALOGID			m_eDialogMode = {DIALOG_END};

//TODO 맵툴 변수 시작
private:
	MAPTAPID	m_eMapTapID = { TAP_END };

private: //! For. Map
	VTXDYNAMIC					m_tMapInfo;
	_float						m_fTileX = { 0.0f };
	_float						m_fTileZ = { 0.0f };
	_bool						m_bTileing = { false };
	_bool						m_bCreate = { false };
	_int						m_iTileMode = { 0 };
	_int						m_iBrushRange = 1.f;
	_int						m_iBrushPower = 1.f;

//TODO 맵툴 변수 종료
// 
//TODO 오브젝트 툴 변수 시작

private: //!For.Object
	_bool						m_bPicking = { false };
	_bool						m_bPressing = { false };

	_float3						m_fPickingPos = { 0.f, 0.f, 0.f };
	_int						m_iObjectMode = { 0 };
	_int						m_iSelectTagIndex = { 0 };
	_int						m_iPickingObjectIndex = 0;

	CGameObject*				m_PickingObject = nullptr;
	vector<CGameObject*>		m_vecObjects;
	vector<string>			m_vecObjectProtoTags;
	
	vector<string>			m_vecCreateObjectTag;
	
	_float*					    m_arrView = { nullptr };
	_float*						m_arrProj = { nullptr };
	_float						snap[3] = { 1.f, 1.f, 1.f };

//TODO 오브젝트 툴 변수 종료

	
//TODO 카메라툴 변수 시작

private:
	map<const char*, _float3> m_MovePoints;
	_int					  m_iPointIndex = { 0 };

//TODO 카메라툴 변수 종료

private:
	void	HelpMarker(const char* desc);
	char* ConverWStringtoC(const wstring& wstr);

private:
	void	ShowMapTool();
	void	ShowObjectTool();
	void	ShowCameraTool();
	void	ShowEffectTool();
	
private:
	void	Picking(TOOLID eToolID, _int iMode);
	
	

public:
	virtual void Free() override;
};
END
