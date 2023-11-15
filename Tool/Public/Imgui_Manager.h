#pragma once
#include "Base.h"
#include "Tool_Defines.h"



BEGIN(Tool)
class CImgui_Manager final : public CBase
{

private:
	CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	~CImgui_Manager() = default;

public:
	HRESULT SetUp_Imgui();
	void	Tick(_float fTimeDelta);
	void	Render();

public:
	static CImgui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


private:
	char*	ConvertWCtoC(const wchar_t* str);
	wchar_t* ConvertCtoWC(const char* str);

private:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_bool				 m_bMainTool = { true };
	_bool				 m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };

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
