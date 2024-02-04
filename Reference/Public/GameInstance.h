#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Component_Manager.h"

/* Ŭ���̾�Ʈ���� ������ ����� ����ϱ����� �ݵ�� ���ľ��ϴ� ��ü. */

/* DX11�� ���� �ʿ��� ��ü�� �����ϰ� �������ϴ� �۾��� �����Ѵ�. */
/* ���� ������ ����(��)�� �����ϰ� �ʿ信���� ������Ʈ ����. */
/* ���� ������ ��ü���� �����ϰ� ������Ʈ�ϰ� �����Ѵ�. */
/* ���� ������ ���� ������Ʈ���� ������ �����ϰ� �����Ͽ� �����´�. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	/* �������̺귯���� ����ϱ����� �غ� ��� ��ģ��. */
	HRESULT Initialize_Engine(_uint iNumLevels, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void	Tick_Engine(_float fTimeDelta);
	HRESULT Render_Engine();
	void	Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */		

	IDXGISwapChain*			Get_SwapChain();
	ID3D11RenderTargetView* Get_BackBufferRTV() const;
	ID3D11DepthStencilView* Get_DSV() const;
	GRAPHIC_DESC			Get_GraphicDesc();

	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	HRESULT Resize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT UseFullScreen(_bool bMode);
	//GRAPHIC_DESC Get_GraphicDesc() const { return m_pGraphic_Device;}
	
public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const wstring& strTimeTag);
	_float Compute_TimeDelta(const wstring& strTimeTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);
	_uint	Get_CurrentLevelIndex();

public: /* For.Object_Manager */
	HRESULT				Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype, _bool bAddData = false, _bool bModelType = false);
	HRESULT				Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr, CGameObject **ppOut = nullptr);
	class CGameObject*  Get_CloneObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent*	Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex = 0);
	class CComponent*	Get_PartComponent(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, const wstring& strPartTag, _uint iIndex = 0);
	class CGameObject*  Get_Player(_uint iLevelIndex);
	HRESULT				Erase_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pEraseObject);
	class CLayer*		Find_Layer(_uint iLevelIndex, const wstring & strLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, class CComponent* pPrototype, _bool bModelCom = false);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void* pArg);

public: /* For.Data_Manager */
	HRESULT Add_PrototypeTag(const wstring & strProtoTypeTag, _bool bModelType);

	map<const wstring, _bool>& Get_ObjectTags();
	vector<wstring>&		   Get_VecTags();

	HRESULT					   Add_LayerTag(const wstring & strLayerTag);
	vector<wstring>&		   Get_LayerTags();

	HRESULT					   Add_ModelTag(const wstring & strModelTag);
	vector<wstring>&		   Get_ModelTags();

	HRESULT						Add_ParticleTextureTag(const wstring & strTextureTag);
	HRESULT						Add_EffectTextureTag(const wstring & strTextureTag);
	HRESULT						Add_EffectMeshTag(const wstring & strMeshModelTag);
	vector<wstring>&			Get_ParticleTags();
	vector<wstring>&			Get_EffectTags();
	vector<wstring>&			Get_EffectMeshTags();

	HRESULT						Add_ModelData(const wstring & strModelDataTag, MODELDATA* ModelData);
	MODELDATA*					Get_ModelData_InKey(const wstring & strModelDataTag);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, class CGameObject* pGameObject);
	
public: /* For.PipeLine */
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix);

	_matrix		Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState);

	_matrix		Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState);

	RAY			Get_Ray(_uint & In_ViewPortWidth, const _uint & In_ViewPortHeight);
	_float4		Get_CamPosition();
	_float4		Get_CamDir();

public: /* For.Input_Device */
	_byte   Get_DIKeyState(_ubyte byKeyID);
	_byte   Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long   Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_bool	Key_Pressing(_ubyte byKeyID);
	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Up(_ubyte byKeyID);
	_bool	Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouse);
	void	Mouse_Fix();

public:	/* For.Font_Manager */
	HRESULT Add_Font(const wstring & strFontTag, const wstring & strFontFilePath);
	HRESULT Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fScale = 1.f, _float2 vOrigin = _float2(0.f, 0.f), _float fRotation = 0.f);

public: /* For.Collision_Manager*/
	HRESULT Add_Check_CollisionGroup(const _tchar * LeftLayerTag, const _tchar * RightLayerTag);
	void	Update_CollisionMgr(_uint iLevelIndex, _float fTimeDelta);
	void	Reset_CollisionGroup();

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor);
	HRESULT Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag);
	HRESULT Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget_ShaderResource(const wstring & strTargetTag, class CShader* pShader, const _char * pConstantName);

#ifdef _DEBUG
	HRESULT Ready_RenderTarget_Debug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_RTVs(const wstring & strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public: /* For.Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	
public: /* For.Event_Manager */
	HRESULT Add_Event(const wstring& strAddEventTag, class CMyEvent* pMyEvent, void* pDesc);
	HRESULT Erase_Event(const wstring& strEraseEventTag);

public: /* For.UI_Manager */
	HRESULT Add_UI(const wstring& strAddUITag, class CMyUI* pMyUI, void* pDesc);
	HRESULT Erase_UI(const wstring& strEraseUITag);

public: /* For.Frustum */
	void	Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool	isIn_WorldPlanes(_fvector vPoint, _float fRadius = 0.f);
	_bool	isIn_LocalPlanes(_fvector vPoint, _float fRadius);
	

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CData_Manager*			m_pData_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };
	class CUI_Manager*				m_pUI_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

public:
	void Release_Manager();
	static void Release_Engine();
};

END