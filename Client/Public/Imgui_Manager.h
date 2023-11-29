#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

private:
	CImgui_Manager();
	virtual ~CImgui_Manager();

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);
	void	Render();

	LEVEL   Get_Level_ID() { return m_eLevelID; }
	void	Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }
	ID3D11Device* GetDevice() { return m_pDevice;}


	void	ResizeImGui(WPARAM wParam, LPARAM lParam);
	void	CreateRenderTarget();
	void	CleanUpRenderTarget();
//TODO For.MapTool Start
public:
	HRESULT	Save_EditTexture();

private:
	ID3D11Texture2D*	m_pTexture2D = { nullptr };

//!	   For.MapTool End


private:	
	_bool	CreateDeviceD3D();
	void	CleanupDeviceD3D();

	char*	ConvertWCtoC(const wchar_t* str);
	wchar_t* ConvertCtoWC(const char* str);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	IDXGISwapChain*	m_pSwapChain = { nullptr };
	ID3D11RenderTargetView* m_pMainRenderTargetView = { nullptr };
	
private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private:

	_bool				 m_bReady = true;
	_bool				 m_bMainTool = { true };
	_bool				 m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };
	LEVEL			     m_eLevelID = { LEVEL_END };
	
	

private:
	void	HelpMarker(const char* desc);
	char* ConverWStringtoC(const wstring& wstr);

private:
	void	ShowMapTool();
	void	ShowEffectTool();
	void	ShowObjectTool();
	void	ShowCameraTool();

};
END
