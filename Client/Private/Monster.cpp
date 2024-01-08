#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice,pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CAnimObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
}

void CMonster::Tick(_float fTimeDelta)
{
}

void CMonster::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	
	return S_OK;
}

void CMonster::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);




}

void CMonster::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);
}

void CMonster::Init_Desc()
{
}

HRESULT CMonster::Ready_BehaviorTree()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
