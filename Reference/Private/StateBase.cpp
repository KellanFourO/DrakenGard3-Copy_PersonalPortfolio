#include "StateBase.h"
#include "GameInstance.h"
#include "Model.h"

CStateBase::CStateBase()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CStateBase::Initialize(class CModel* pOwnerModel)
{
	m_pOwnerModelCom = pOwnerModel;

	if(AddRefIfNotNull(m_pOwnerModelCom))
		return E_FAIL;

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

void CStateBase::Tick(const _float& fTimeDelta)
{
}

void CStateBase::Late_Tick(const _float& fTimeDelta)
{
	
}

void CStateBase::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerModelCom);
	Safe_Release(m_pGameInstance);
}
