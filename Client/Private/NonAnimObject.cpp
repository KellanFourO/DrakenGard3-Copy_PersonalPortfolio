#include "stdafx.h"
#include "NonAnimObject.h"
#include "GameInstance.h"

CNonAnimObject::CNonAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CNonAnimObject::CNonAnimObject(const CNonAnimObject& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

_bool CNonAnimObject::MouseOnThis()
{
 	if (nullptr == m_pModelCom)
 	{
 		MSG_BOX("ModelCom is nullptr");
 		return false;
 	}
 
 	_uint WinCX, WinCY;
 	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;
 
 	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);
 
 	if (m_pModelCom->Compute_MousePos(WorldRay, m_pTransformCom->Get_WorldMatrix()))
 	{
 		return true;
 	}

	return false;
}

void CNonAnimObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}


