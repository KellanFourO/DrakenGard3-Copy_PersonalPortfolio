#pragma once

#include "Renderer.h"

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
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT  Render_Engine();
	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */		
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	HRESULT Resize(UINT iWidth, UINT iHeight);
	
public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const wstring& strTimeTag);
	_float Compute_TimeDelta(const wstring& strTimeTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, class CGameObject* pGameObject);
	

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };

public:
	void Release_Manager();
	static void Release_Engine();
};

END