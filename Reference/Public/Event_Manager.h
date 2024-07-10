#pragma once
#include "Base.h"

BEGIN(Engine)


class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Event(const wstring& strAddEventTag, CMyEvent* pMyEvent, void* pDesc);
	HRESULT Erase_Event(const wstring& strEraseEventTag);

public:
	void	Tick(_float fTimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render_Events();
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	
private:
	map<const wstring, class CMyEvent*>		m_Events;

private:
	class CMyEvent*			Find_Event(const wstring& strEventTag);

public:
	static CEvent_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END