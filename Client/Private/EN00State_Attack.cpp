#include "EN00State_Attack.h"
#include "GameInstance.h"
#include "Animation.h"

//!m_pOwnerModelCom->Set_Animation(0);
//! m_fSearchRange = 10.f;
//!m_fAttackRange = 10.f;
//!m_pOwner = pOwner;
//!
//!m_pOwnerTransform->Set_SpeedPerSec(3.f);
//!m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(70.0f));

BrainTree::Node::Status CEN00State_Attack::update()
{

	fConsoleDebugAcc += blackboard->GetTimeDelta();

	SetAnimation(7);


	if (fConsoleDebugAcc > 2.f)
	{
		cout << "EN00 - BrainTree - Attack" << endl;
		fConsoleDebugAcc = 0.f;
	}


	if (true == blackboard->GetModel()->Get_CurrentAnimation()->Get_Finished())
	{
		return BrainTree::Node::Status::Success;
	}

	else if (false == AttackEnable(m_vDistance, "Attack_Range"))
	{
		return BrainTree::Node::Status::Failure;
	}
	
}