#pragma once
#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Client)

class CPlayerPart_Weapon final : public CPartObject
{
private:
	CPlayerPart_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerPart_Weapon(const CPlayerPart_Weapon& rhs);
	virtual ~CPlayerPart_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc();

public:
	/* 원형객체를 생성한다. */
	static CPlayerPart_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

