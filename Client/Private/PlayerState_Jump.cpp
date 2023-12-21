#include "PlayerState_Jump.h"

#include "Model.h"
#include "Player.h"
#include "PlayerPart_Body.h"
#include "GameInstance.h"

CPlayerState_Jump::CPlayerState_Jump()
{
}

HRESULT CPlayerState_Jump::Initialize(CPlayer* pPlayer)
{
	m_isGround = false;
	m_iAnimIndex = 0;
	m_fEndTime = 0.5f; //TODO ���� ����

	m_eStateType = CStateMachine::STATE_AIR;

	m_strName = TEXT("PlayerState_Jump");

	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_pOwnerModelCom->Set_Animation(m_iAnimIndex);

	return S_OK;
}

HRESULT CPlayerState_Jump::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Jump::Tick(const _float& fTimeDelta)
{

	return S_OK;
}

HRESULT CPlayerState_Jump::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Jump::Transition()
{
	
	
	return S_OK;
}

void CPlayerState_Jump::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Jump"));	
	}

	//TODO ���� �� �ڽ��� ���� �ִ�Ÿ��� ���� �� ���� ���������� �߶� ��� ���� �ٲ��
	//! �߶� �� ��� Ű�Է��� �޾� �ٸ� ���·� ó���� ��.

}

void CPlayerState_Jump::ResetState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;
}

CPlayerState_Jump* CPlayerState_Jump::Create(CPlayer* pPlayer)
{
	CPlayerState_Jump* pInstance = new CPlayerState_Jump();

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Jump");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Jump::Free()
{
	__super::Free();
}
