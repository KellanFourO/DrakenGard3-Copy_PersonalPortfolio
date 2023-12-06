#include "stdafx.h"
#include "AnimObject.h"

CAnimObject::CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAnimObject::CAnimObject(const CAnimObject& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}
