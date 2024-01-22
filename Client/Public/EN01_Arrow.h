#pragma once
#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CEN01_Arrow final : public CBullet
{
public:
	enum EN01_ARROWTYPE { ARROW_NORMAL, ARROW_PARABOLIC, ARROW_C_NORMAL, ARROW_C_PARABOLIC, ARROW_END };

public:
	typedef struct tagEN01_ArrowDesc : public CBullet::BULLET_DESC
	{
		EN01_ARROWTYPE eArrowType = ARROW_END;

	}EN01_ARROWDESC;

private:
	CEN01_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEN01_Arrow(const CEN01_Arrow& rhs);
	virtual ~CEN01_Arrow() = default;


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
	
private:
	EN01_ARROWTYPE	m_eArrowType = ARROW_END;

	

public:
	/* 원형객체를 생성한다. */
	static CEN01_Arrow* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};


END
