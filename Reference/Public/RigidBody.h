#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;

public:
	void	Set_Force(_float3 _vForce) { m_vNetPower = _vForce; }
	void	Add_Force(_float3 _vForce);

	void	Set_Heavy(_float _Heavy) { m_fHeavy = _Heavy; }

	void	IsKinematic(_bool _bKinematic) { m_isKinematic = _bKinematic; }
	void	UseGravity(_bool _bGravity) { m_UseGravity = _bGravity; }
	void	UseGravity(_bool _bGravity, _float _fHeavy) { m_UseGravity = _bGravity; m_fHeavy = _fHeavy; }

	_bool	IsKinematic() { return m_isKinematic; }
	_bool	UseGravity() { return m_UseGravity; }

private:
	class CTransform*		m_pOwnerTransformCom = { nullptr };

private:
	_bool					m_isKinematic = true;
	_bool					m_UseGravity = true;
	_float3					m_vNetPower;

	_float					m_fGravityAcc = 9.8f * -9.8f * 1.3f;
	_float					m_fHeavy = 9.8f;

public:
	static CRigidBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
	
};

END

