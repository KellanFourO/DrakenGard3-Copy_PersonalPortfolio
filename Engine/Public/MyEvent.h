#pragma once
#include "Base.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CGameInstance;

class ENGINE_DLL CMyEvent abstract : public CBase
{
public:
	enum EVENTTYPE { BOSS1_APPEAR, EVENTTYPE_END };

	typedef struct tagEventDesc
	{
		EVENTTYPE eEventType = EVENTTYPE_END;
		_uint	  iCurrentLevelIndex = 0;
		_float3   vColliderSize = {};
		_float3   vColliderCenter = {};
		_float3	  vEventPos = {};

	}EVENT_DESC;

protected:
	CMyEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyEvent() = default;

public:
	void	StartEvent() { m_bStart = true; }
	void	EndEvent() { m_bEnd = true;}
	_bool	IsEnd() { return m_bEnd; }
	virtual void	Play_Event(EVENTTYPE eEventType);

public:
	virtual HRESULT Initialize(EVENT_DESC* pDesc);
	virtual void	Tick(_float fTimeDelta);
#ifdef _DEBUG
	virtual HRESULT Render();
#endif
	virtual HRESULT		Ready_Components();


protected:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	CCollider*		m_pColliderCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CGameInstance*  m_pGameInstance = { nullptr };
	
protected:
	CCollider*		m_pTargetCollider = { nullptr };

protected:
	_bool m_bStart = false;
	_bool m_bEnd = false;
	_bool m_bFindCollider = false;
	
	EVENT_DESC m_tEventDesc = {};

public:
	//static	CMyEvent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, EVENT_DESC* pDesc);
	virtual void	  Free() override;
};

END

