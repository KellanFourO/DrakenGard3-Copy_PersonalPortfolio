#pragma once
#include "Base.h"

BEGIN(Engine)


class CUI_Manager final : public CBase
{
private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_UI(const wstring& strAddUITag, CMyUI* pMyUI, void* pDesc);
	HRESULT Erase_UI(const wstring& strEraseUITag);

public:
	void	Tick(_float fTimeDelta);


public:
	HRESULT Render_UIs();


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	
private:
	map<const wstring, class CMyUI*>		m_UIs;

private:
	class CMyUI*			Find_UI(const wstring& strUITag);

public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END