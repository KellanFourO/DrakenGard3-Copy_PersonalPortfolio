#pragma once

#include "Component.h"

BEGIN(Engine)
class CGameObject;
class CNavigation;


class ENGINE_DLL CRigidBody final : public CComponent
{
	public:
		enum AXIS_TYPE { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
		enum class RIGIDBODY_TYPE { STATIC, DYNAMIC, TYPEEND };
		enum class FORCE_MODE { FORCE, IMPULSE, ACCELERATION, VELOCITY_CHANGE, TYPEEND };
		/*
			FORCE : 바람이나, 자기력과 같이 연속적으로 주어지는 힘

			IMPULSE : 타격이나 폭발과 같은 순간적인 힘 (충격량)

			ACCELERATION : 질량 무시, 직접 가속량 부여
							위 두 모드는 질량에 따라 움직임이 달라지지만, 이 모드는 질량에 관계업이 일정한 가속을 만들어 낸다.

			VELOCITY_CHANGE : 질량 무시 직접 속도 변화 부여
								ACCELERATION은 시간이 흘러가면서 변화를 일으키는데 비해 이 모드는 순간적으로 지정한 속도로 변화를 일으킨다.

		*/

	public:
		CRigidBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
		CRigidBody(const CRigidBody& rhs);
		virtual ~CRigidBody() = default;

	public:
		virtual HRESULT			Initialize_Prototype();
		virtual HRESULT			Initialize(void* pArg) override;
		virtual void			Tick(_float fTimeDelta) override;
		//void					LateTick(const _float& fTimeDelta);

	public:
		const RIGIDBODY_TYPE&	Get_Type() const { return m_eType; }
		const _float&			Get_Mass() const { return m_fMass; }
		const _float&			Get_Friction() const { return m_fFriction; }
	
		_float3					Get_LinearVelocity() const { return m_vLinearVelocity; }
		_float					Get_LinearAxisVelocity(AXIS_TYPE eAxis) const { return *((_float*)&m_vLinearVelocity + (_int)eAxis); }
	
	public:
		void					Set_Owner(class CGameObject* pOwner) { m_pOwner = pOwner; }
		void					Set_OwnerNavigation(CNavigation* pNavigation) { m_pOwnerNavigation = pNavigation; }
		void					Set_Kinematic(const _bool & bKinematic) { m_bKinematic = bKinematic; }
		void					Set_UseGravity(const _bool & bUse) { m_bUseGravity = bUse; }
		void					Set_Gravity(const _float & fGravity) { m_fGravitionalConstant = fGravity; }
		void					Set_Mass(const _float & fMass) { m_fMass = fMass; }
		void					Set_Friction(const _float & fFriction) { m_fFriction = fFriction; }
	
		void					Reset_OneTick() { m_bOneTick = true; }
		void					Reset_AccelZero() { m_bAccelerationZero = false; }
		void					Set_LinearVelocity(const _float3& vLinearVelocity) { m_vLinearVelocity = vLinearVelocity; }
		void					Set_LinearAxisVelocity(AXIS_TYPE eAxis, _float fVelocity) { *((_float*)&m_vLinearVelocity + (_int)eAxis) = fVelocity; }
		void					Set_FreezePosition(AXIS_TYPE eAxis) { m_byConstraints ^= 1 << (_int)eAxis; }
	
	public:
		const _bool&			Is_AccelZero() { return m_bAccelerationZero; }
		const _bool&			Is_UseGravity() const { return m_bUseGravity; }
		const _bool&			Is_Kinematic() const { return m_bKinematic; }
		_bool					Is_FrozePosition(AXIS_TYPE eAxis) { return m_byConstraints & 1 << (_int)eAxis; }
	
	public:
		void					Add_Force(const _float3& vForce, const FORCE_MODE & eMode);
		void					Clear_Force(const FORCE_MODE & eMode);
		void					Clear_NetPower();
	
	private:
		void					Update_Kinetic(const _float & fTimeDelta);
		void					Update_Kinematic(const _float & fTimeDelta);
		void					Update_Transform(const _float & fTimeDelta);
	
	private:
		const _bool				Check_Sleep();
		void					Sleep() { Clear_NetPower(); m_bSleep = TRUE; }
		void					Wake() { m_bSleep = FALSE; }

	private:
		RIGIDBODY_TYPE			m_eType = RIGIDBODY_TYPE::TYPEEND;
		_bool					m_bSleep = FALSE;
		
		
		_bool					m_bUseGravity = FALSE;
		_bool					m_bKinematic = FALSE;

		_bool					m_bOneTick = false;
		_bool					m_bAccelerationZero = false;
		_float					m_fFriction = 0.1f;
		_float					m_fMass = 10.f;
		//_float					m_fGravitionalConstant = -9.8f;
		_float					m_fGravitionalConstant = -14.8f;
	
		_byte					m_byConstraints = 0;
	
		_float3					m_vForce; //! 충격 당했을때를 기록해주자.
		_float3					m_vLinearAcceleration;
		_float3					m_vLinearVelocity;
	
		_float					m_fSleepThreshold = 0.05f;
	
		CGameObject*			m_pOwner = { nullptr };
		CNavigation*			m_pOwnerNavigation = { nullptr };

	public:
		static	CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
		virtual CComponent* Clone(void* pArg) override; //! 복사를 강제한다
		virtual void Free() override;
};

END