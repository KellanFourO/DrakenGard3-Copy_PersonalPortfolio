#include "stdafx.h"
#include "NonAnimObject.h"

CNonAnimObject::CNonAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CNonAnimObject::CNonAnimObject(const CNonAnimObject& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}
