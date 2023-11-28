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
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick();
	void	Render();

	_bool   Get_Ready() { return m_bReady;}
	void    Set_Ready(_bool bReady) { m_bReady = bReady; }

	LEVEL   Get_Level_ID() { return m_eLevelID; }
	void	Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }
	

private:
	char*	ConvertWCtoC(const wchar_t* str);
	wchar_t* ConvertCtoWC(const char* str);

private:
	_bool				 m_bReady = false;

	_bool				 m_bMainTool = { true };
	_bool				 m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };
	LEVEL			     m_eLevelID = { LEVEL_END };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

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
