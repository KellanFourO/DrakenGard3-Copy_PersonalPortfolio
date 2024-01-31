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




void CNonAnimObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBuffer_Instance);
}


