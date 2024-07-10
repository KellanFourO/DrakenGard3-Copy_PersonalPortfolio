#pragma once
#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Engine)
class CCollider;
class CTexture;
END

BEGIN(Client)
class CEffect_Trail;

class CPlayerPart_Weapon final : public CPartObject
{
private:
	CPlayerPart_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerPart_Weapon(const CPlayerPart_Weapon& rhs);
	virtual ~CPlayerPart_Weapon() = default;

	
public:
	void	On_Trail();
	void	Off_Trail();
	void	Set_BloodyMode(_bool bBloodyMode) { m_bBloodyMode = bBloodyMode;}
	
	CEffect_Trail* Get_Trail() { return m_pTrail;}
	

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc();



private:
	CCollider*			m_pColliderCom = { nullptr };
	CCollider*			m_pColliders[6] = {};
	CModel*				m_pBloodyModelCom = { nullptr };
	
	CEffect_Trail*		m_pTrail = { nullptr };
	CEffect_Trail*		m_pBloodyTrail = { nullptr };

	_bool				m_bBloodyMode = false;
public:
	/* ������ü�� �����Ѵ�. */
	static CPlayerPart_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

