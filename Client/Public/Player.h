#pragma once

#include "Client_Defines.h"
#include "AnimObject.h"
#include "PartObject.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CAnimObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CPartObject*		Find_PartObject(const wstring& strPartTag);

private:
	CNavigation*		m_pNavigationCom = { nullptr };

private:
	_int				m_iCurrentAnimIndex = { 3 };

private:
	map<const wstring, class CPartObject*>	m_PartObjects;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);

public:
	/* ������ü�� �����Ѵ�. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END