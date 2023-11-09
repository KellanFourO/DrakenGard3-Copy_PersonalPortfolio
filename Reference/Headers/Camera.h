#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera() = default;

protected:
	ID3D11Device*	m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	virtual void	Free() override;
};

END
