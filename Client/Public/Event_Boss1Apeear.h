#pragma once
#include "MyEvent.h"


BEGIN(Client)

class CEvent_Boss1Appear final : public CMyEvent
{
private:
			 CEvent_Boss1Appear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent_Boss1Appear() = default;

public:
	virtual HRESULT Initialize(EVENT_DESC* pDesc) override;
	virtual void	Tick(_float fTimeDelta) override;
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif


public:
	virtual void	Play_Event(EVENTTYPE eEventType) override;
	virtual HRESULT Ready_Components() override;


private:
	class CBoss_EN131* m_pBoss = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	_bool m_bFindObject = false;
	


public:
	/* 원형객체를 생성한다. */
	static CEvent_Boss1Appear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, EVENT_DESC* pDesc);

	virtual void Free() override;

};

END

