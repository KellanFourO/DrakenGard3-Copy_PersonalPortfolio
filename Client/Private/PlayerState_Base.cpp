#include "stdafx.h"
#include "PlayerState_Base.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Navigation.h"
#include "StateMachine.h"

CPlayerState_Base::CPlayerState_Base()
{
}

void CPlayerState_Base::KeyInput(const _float& fTimeDelta)
{
}

void CPlayerState_Base::Free()
{
	__super::Free();

	
	Safe_Release(m_pOwnerStateCom);
	Safe_Release(m_pOwnerTransform);
	Safe_Release(m_pOwnerRigidBody);
	Safe_Release(m_pOwnerNavagation);

	
}
