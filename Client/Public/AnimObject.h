#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CAnimObject abstract : public CGameObject
{
protected:
	CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimObject(const CAnimObject& rhs);
	virtual ~CAnimObject() = default;


protected:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };

};

END

