
#include "StateBase.h"
#include "Model.h"

CStateBase::CStateBase()
{
}

HRESULT CStateBase::Initialize()
{
	return S_OK;
}

HRESULT CStateBase::StartState()
{
	return S_OK;
}

HRESULT CStateBase::EndState()
{
	return S_OK;
}

void CStateBase::Priority_Tick(const _float& fTimeDelta)
{
	
}

void CStateBase::Late_Tick(const _float& fTimeDelta)
{
	
}

void CStateBase::Free()
{
	Safe_Release(m_pOwnerModelCom);
}
