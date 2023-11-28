#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CCamera_MapTool final : public CCamera
{
public:
	typedef struct tagDynamicCameraDesc : public CCamera::tagCameraDesc
	{
		_float fMouseSensitivity = 0.0f;
	}DYNAMIC_CAMERA_DESC;

private:
	CCamera_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_MapTool(const CCamera_MapTool& rhs);
	virtual ~CCamera_MapTool() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

private:
	void	KeyInput(_float fTimeDelta);
	void	MouseInput(_float fTimeDelta);

private:
	_float			m_fMouseSensitivity = { 0.0f };
	LEVEL			m_eCurrentLevelID = { LEVEL_END };

public:
	static CCamera_MapTool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

END

