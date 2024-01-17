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

private:
	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Target(const CCamera_Target& rhs);
	virtual ~CCamera_Target() = default;

public:
	void	Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

private:
	void	KeyInput(_float fTimeDelta);
	_vector	MouseInput(_float fTimeDelta);
	

private:
	_float			m_fMouseSensitivity = { 0.0f };
	LEVEL			m_eCurrentLevelID = { LEVEL_END };
	_bool			m_bAdmin = false;

	CGameObject*	m_pTarget = { nullptr };

	_float			m_fSpringConstant; //! ���ö ���, �þ���� �������� ź���� �پ���.
	_float			m_fDampConstant; //! ���ö ����� ���������� ����(Dampening) ���

	_float3			m_vVelocity; //! �ӵ�����
	_float3			m_vLookVelocity; //!��ӵ�����

	_float			m_fMouseX = 0, m_fMouseY = 0;

	_float3			m_vOffset;
	_float3			m_vActualPos;

	_bool			m_bMouseFix = true;



public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

END

