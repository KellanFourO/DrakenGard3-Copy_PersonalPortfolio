#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

private:
			 CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);
	void	Render();

	LEVEL		  Get_Level_ID() { return m_eLevelID; }
	void		  Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }
	
//TODO For.MapTool Start
public:
	HRESULT	Save_EditTexture();
	

private:
	ID3D11Texture2D*	m_pTexture2D = { nullptr };
	char*		ConvertWCtoC(const wchar_t* str);
	wchar_t*	ConvertCtoWC(const char* str);

private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CDynamic_Terrain*		m_pDynamic_Terrain = { nullptr };

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_bReady = true;
	_bool					m_bMainTool = { true };
	_bool					m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };
	LEVEL					m_eLevelID = { LEVEL_END };
	
	RAY						m_tRay;

private: //! For. MapTool
	VTXDYNAMIC	m_tMapInfo;
	_float		m_fTileX = { 0.0f };
	_float		m_fTileZ = { 0.0f };
	


private:
	void	HelpMarker(const char* desc);
	char* ConverWStringtoC(const wstring& wstr);

private:
	void	ShowMapTool();
	void	ShowEffectTool();
	void	ShowObjectTool();
	void	ShowCameraTool();


public:
	virtual void Free() override;
};
END
