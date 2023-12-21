#include "PlayerState_Base.h"
#include "GameInstance.h"
#include "Player.h"
#include "Model.h"
#include "Navigation.h"
#include "Transform.h"
#include "PartObject.h"

CPlayerState_Base::CPlayerState_Base()
{
}

HRESULT CPlayerState_Base::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	if (FAILED(AddRefIfNotNull(m_pPlayer)))
		return E_FAIL;

	m_pOwnerModelCom = dynamic_cast<CModel*>(m_pPlayer->Find_PartObject(TEXT("Part_Body"))->Find_Component(TEXT("Com_Model")));
	
	if(FAILED(AddRefIfNotNull(m_pOwnerModelCom)))
		return E_FAIL;
	
	m_pOwnerNavigationCom = dynamic_cast<CNavigation*>(m_pPlayer->Find_Component(TEXT("Com_Navigation")));

	if (FAILED(AddRefIfNotNull(m_pOwnerNavigationCom)))
		return E_FAIL;

	m_pOwnerTransformCom = m_pPlayer->Get_Transform();

	if (FAILED(AddRefIfNotNull(m_pOwnerTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerState_Base::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Base::Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Base::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}



HRESULT CPlayerState_Base::Transition()
{
	return S_OK;
}

void CPlayerState_Base::KeyInput(const _float& fTimeDelta)
{
}

void CPlayerState_Base::ResetState()
{
}


void CPlayerState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerNavigationCom);
	Safe_Release(m_pOwnerTransformCom);
	
}
