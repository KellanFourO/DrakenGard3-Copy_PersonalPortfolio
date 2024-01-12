#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

public:
	enum TOOLID { TOOL_MAP, TOOL_OBJECT, TOOL_NAVI, TOOL_CAMERA, TOOL_EFFECT, TOOL_END };
	enum DIALOGID { DIALOG_SAVE, DIALOG_LOAD, DIALOG_BINARY, DIALOG_END };
	enum BRUSHMODE { BRUSH_DOWN, BRUSH_UP, BRUSH_PRESSING, BRUSH_END };

private:
			 CImgui_Manager();
	virtual ~CImgui_Manager() = default;
	
public:
	LEVEL					Get_Level_ID() { return m_eLevelID; }
	void					Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }
	class CCamera_MapTool** Get_Cam() { return &m_pCamera; }

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Tick(_float fTimeDelta);
	void					Render();

private:
	HRESULT					ImGui_Initialize();
	void					ImGuizmo_Initialize();
	void					ImGuiFileDialog_Intialize();

private:
	void					ImGui_MainTick();
	void					ImGui_MapToolTick();
	void					ImGui_ObjectToolTick();

private:
	_bool					ImGui_MouseInCheck();
	void					UpdateRay();

//TODO Imguizmo #�����
//TODO ImguiDialog #���̾�α�
//TODO ���� #����
//TODO ������Ʈ�� #������Ʈ��
//TODO �L���ڿ� #���ڿ��Լ�	 

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CDynamic_Terrain* m_pDynamic_Terrain = { nullptr };
	class CField*	m_pField = { nullptr };
	class CCamera_MapTool* m_pCamera = { nullptr };
	class CNavigation*		m_pNavigation = { nullptr };
	
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	LEVEL					m_eLevelID = { LEVEL_END };
	TOOLID					m_eToolID = { TOOL_END };


private: //TODO Imguizmo #�����
	void					Set_Guizmo();
	void					Set_GuizmoCamView();
	void					Set_GuizmoCamProj();

private:
	_float*						m_arrView = { nullptr };
	_float*						m_arrProj = { nullptr };
	_float						snap[3] = { 1.f, 1.f, 1.f };
		 

private: //TODO ImguiDialog #���̾�α�
		void				OpenDialog(TOOLID eToolID);
		void				ShowDialog(TOOLID eToolID);
		void				OpenDialogBinaryModel();
		void				ShowDialogBianryModel(MODEL_TYPE eModelType);

private:
		
		string						m_strCurrentDialogTag;
		DIALOGID					m_eDialogMode = { DIALOG_END };
		MODEL_TYPE					m_eModelType = { MODEL_TYPE::TYPE_END };
		
		

private: //TODO ���� #���� 
	void					PickingTerrain(BRUSHMODE eBrushMode);
	void					SaveMap(string strFilePath, string strFileName);
	void					LoadMap(string strFilePath, string strFileName);

private:
	_bool						m_bReady = true;
	RAY							m_tWorldRay = {};

private:
	VTXDYNAMIC					m_tMapInfo;
	VTXFIELD					m_tFieldInfo;
	_bool						m_bMapToolPickMode = { false };
	BRUSHMODE					m_eBrushMode = { BRUSHMODE::BRUSH_END };

	_int						m_iTileMode = { 0 };
	_float						m_fTileX = { 0 }, m_fTileZ = { 0 };

	_int						m_iBrushRange = 1.f, m_iBrushPower = 1.f;
		 

private: //TODO ������Ʈ�� #������Ʈ��
	HRESULT					Add_PrototypeTag(const wstring& strPrototypeTag, _bool bModelType);
	HRESULT					Ready_ProtoTagList();

	void					BinaryModeTick();
	void					ObjectModeTick();

	void					CreateObjectFunction();
	void					SelectObjectFunction();
	void					DeleteObjectFunction();

	void					SaveObject(string strPath, string strFileName);
	void					LoadAnimObject(string strPath, string strFileName);
	void					LoadNonAnimObject(string strPath, string strFileName);

	void					ClearAnimObjects();
	void					ClearNonAnimObjects();

	HRESULT					StartBakeBinary();
	HRESULT					BinaryConvert(string strFileName, string strFilePath, const MODEL_TYPE& eModelType);
	HRESULT					ReadFBX(string strFilePath, const MODEL_TYPE& eModelType);

	HRESULT					Read_BoneData(aiNode* pAINode, _int iIndex, _int iParentIndex, _int iDepth);
	HRESULT					Write_BoneData(string strFileName);

	HRESULT					Read_MeshData(const MODEL_TYPE& eModelType);
	HRESULT					Write_MeshData(string strFileName);

	HRESULT					Read_MaterialData();
	HRESULT					Write_MaterialData(string strFileName);

	HRESULT					Read_AnimationData();
	HRESULT					Write_AnimationData(string strFileName);


	HRESULT					CreateModelInfo(const MODEL_TYPE& eModelType, string strFileName);

	_uint					Get_BoneIndex(const char* szName);

private: 
	_bool						m_bObjectToolPickMode;
	_bool						m_bPressing;

	_int						m_iObjectMode = { 0 };
	_int						m_iModelType = { 0 };

	vector<string>				m_vecLayerTags;
	_int						m_iSelectLayerTagIndex = { -1 };
	_bool						m_bOpenLayerTags = true;

	vector<string>				m_vecAnimObjectTags;
	vector<string>				m_vecCreateAnimObjectTags;
	vector<string>				m_vecCreateAnimObjectLayerTag;

	vector<class CGameObject*>  m_vecAnimObjects;

	vector<string>				m_vecModelTags;

	vector<string>				m_vecNonAnimObjectTags;
	vector<string>				m_vecCreateNonAnimObjectTags;
	vector<string>				m_vecCreateNonAnimObjectLayerTag;
	vector<class CGameObject*>  m_vecNonAnimObjects;
	

	_bool						m_bModelPicking = false;
	_bool						m_bDeleteMode = false;

	_int						m_iSelectTagIndex = { 0 };


	class CGameObject*			m_PickingObject = nullptr;
	class CGameObject*			m_PartObject = nullptr;
	_int						m_iPickingObjectIndex = 0;
	_float3						m_fPickingPos = { 0.f, 0.f, 0.f };

private:
	const aiScene*				m_pAiScene = { nullptr };
	Assimp::Importer			m_Impoter;
	vector<asBone*>				m_vecBones;
	vector<asMesh*>				m_vecMesh;
	vector<asMaterial*>			m_vecMaterial;
	vector<asAnimation*>		m_vecAnimation;

private: //TODO ������̼� �� #������̼���
	void					ImGui_NaviToolTick();
	void					Create_Navi_Mode_Tick();
	void					Delete_Navi_Mode_Tick();
	void					Set_CCW(_float3* vPoint);
	void					Reset_NaviPicking();

	void					SaveNavi(string strFullPath);
	void					LoadNavi(string strFullPath);

private:
	_int						m_iNaviToolMode = 0;
	_int						m_iCurrentPickingIndex = 0;
	_int						m_iTargetIndex = 0;
	_int						m_iNaviIndex = 0;
	_float3						m_fNaviPickingPos;

	class CGameObject*			m_pNaviTargetObject = nullptr;
	
	vector<double>				m_vecNaviPoints;

private:
	string sourceUpperPath = "../Assets/";
	string destUpperPath = "../../Client/Bin/Resources/Models/";
	

private: //TODO ���ڿ� #���ڿ��Լ�
	//!string					ConvertWstrToStr(const wstring& str);
	//!wstring					ConvertStrToWstr(const string& str);
	wstring					SliceObjectTag(const wstring& strObjectTag);
	void					Replace(string& str, string comp, string rep);
	vector<string>			Get_AllFolderNames(const string& strDirPath);
	string					CheckOrCreatePath(const string& strPath);
	string					ConvertWstrToStr(const wstring& wstr);
	wstring					ConvertStrToWstr(const string& str);
		 

public:
	virtual void			Free() override;

};
END
