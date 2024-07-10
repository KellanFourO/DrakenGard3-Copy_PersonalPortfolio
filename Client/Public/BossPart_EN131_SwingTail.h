#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CBossPart_EN131_SwingTail final : public CPartObject
{
private:
	CBossPart_EN131_SwingTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossPart_EN131_SwingTail(const CBossPart_EN131_SwingTail& rhs);
	virtual ~CBossPart_EN131_SwingTail() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CCollider* Get_Collider() { return m_pColliderCom; }
	

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc();


private:
	CCollider* m_pColliderCom = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CBossPart_EN131_SwingTail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

