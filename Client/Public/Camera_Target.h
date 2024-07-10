#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CCamera_Target final : public CCamera
{
public:
	typedef struct tagTargetCameraDesc : public CCamera::tagCameraDesc
	{
		_float		 fMouseSensitivity = 0.0f;
		CGameObject* pTarget = { nullptr };
	}TARGET_CAMERA_DESC;

	typedef struct tagCutSceneDesc
	{
		_bool			bCutSin = false; //! true가 되면
		_float3			vStartPos = {}; //! 시작위치
		_float3			vChasePos = {}; //! 지정한 위치로 천천히 추적
		_float			fStopRange = 0.f; //! 추적을 멈출 범위
		_float			fChaseSpeed = 0.f; //! 추적 스피드
		_float			fStartAngle = -1.5f;
		
		CGameObject*	pChaseTarget = nullptr;
	}CUTSCENE_DESC;

private:
	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Target(const CCamera_Target& rhs);
	virtual ~CCamera_Target() = default;

public:
	void	Set_Target(CGameObject* pTarget) { m_pOriginTarget = m_pTarget; m_pTarget = pTarget; }
	void	Set_CutSceneDesc(CUTSCENE_DESC* Desc) { m_tCutScene = *Desc; m_tCutScene.bCutSin = true; }
	void    Set_OffSet(_float3 vOffset) { m_vOriginOffset = m_vOffset; m_vOffset = vOffset; }
	void	Reset_OffSet() { m_vOffset = m_vOriginOffset; }
	
	void	On_Shake(_float fShakePower, _float fShakeDuration);
	
	void	CameraShake(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	
	void			Start_CutScene(_float fTimeDelta);
	void			Init_CutScene();
	void			Reset_CutScene();
	void			Return_Player();

	

private:
	void	KeyInput(_float fTimeDelta);
	_vector	MouseInput(_float fTimeDelta);
	

private:
	_float			m_fMouseSensitivity = { 0.0f };
	LEVEL			m_eCurrentLevelID = { LEVEL_END };
	_bool			m_bAdmin = false;

//! 카메라 셰이크
	_bool			m_bShake = false;
	_float			m_fShakeTimeAcc = 0.f;
	_float			m_fShakePower = 0.f;
	_float			m_fShakeDuration = 0.f;
	random_device	m_RandomDevice; // 랜덤을 뽑기위함
	mt19937_64		m_RandomNumber;
//! 카메라셰이크 끝
	

	CGameObject*	m_pTarget = { nullptr };
	CGameObject*	m_pOriginTarget = {nullptr };
	

	_float			m_fSpringConstant; //! 용수철 상수, 늘어날수록 스프링의 탄력이 줄어든다.
	_float			m_fDampConstant; //! 용수철 상수를 바탕으로한 감쇠(Dampening) 상수

	_float3			m_vVelocity; //! 속도벡터
	_float3			m_vLookVelocity; //!룩속도벡터

	_float			m_fMouseX = 0, m_fMouseY = 0;

	_float3			m_vOffset;
	_float3			m_vOriginOffset;
	_float3			m_vActualPos;


	_bool			m_bMouseFix = true;
	

//! 컷신 전용
	CUTSCENE_DESC   m_tCutScene;
	_float4			m_vOriginPos; //! 컷신 시작전 기존 위치
	_bool			m_bSaveOriginPos = false;
	_bool			m_bTargetChange = false;

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

END

