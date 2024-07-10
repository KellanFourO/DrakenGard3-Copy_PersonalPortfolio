#pragma once
#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CEN131_Breath final : public CBullet
{
public:
	enum EN131_BREATHTYPE { BREATH_LEFT, BREATH_CENTER, BREATH_RIGHT, BREATH_END};

public:
	typedef struct tagEN131_BreathDesc : public CBullet::BULLET_DESC
	{
		EN131_BREATHTYPE eBreathType = BREATH_END;
		_float4			 vParentLook = {};

	}EN131_BREATHDESC;

private:
	CEN131_Breath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEN131_Breath(const CEN131_Breath& rhs);
	virtual ~CEN131_Breath() = default;


public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override; // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;

public:
	virtual void					Initialize_Pos(_fvector vIntializePos) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	Create_Effect();


private:
	_bool  m_bCreateEffect = false;
	EN131_BREATHTYPE	m_eBreathType = BREATH_END;
	_float4				m_vParentLook = {};
	_float m_fEffectCreateTime = 0.f;
	_float m_fEffectTimeAcc = 0.f;
	



public:
	/* ������ü�� �����Ѵ�. */
	static CEN131_Breath* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};


END
