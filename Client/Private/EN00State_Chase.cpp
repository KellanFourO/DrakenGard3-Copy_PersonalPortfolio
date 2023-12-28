#include "EN00State_Chase.h"
#include "GameInstance.h"

//!m_pOwnerModelCom->Set_Animation(0);
//! m_fSearchRange = 10.f;
//!m_fAttackRange = 10.f;
//!m_pOwner = pOwner;
//!
//!m_pOwnerTransform->Set_SpeedPerSec(3.f);
//!m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(70.0f));

BrainTree::Node::Status CEN00State_Chase::update()
{

	SetAnimation(3);
	

	fConsoleDebugAcc += blackboard->GetTimeDelta();
	
	if (fConsoleDebugAcc > 2.f)
	{
		cout << "EN00 - BrainTree - Chase " << endl;
		fConsoleDebugAcc = 0.f;
	}

	XMStoreFloat3(&m_vDistance, Chase(blackboard->GetTarget()));

	if (true == Detect(blackboard->GetTarget(), "Detect_Range") && true == AttackEnable(m_vDistance, "Attack_Range"))
	{
		cout << "EN00 - BrainTree - Attack " << endl;

		return BrainTree::Node::Status::Success;
	}

	else if (false == Detect(blackboard->GetTarget(), "Detect_Range"))
	{

		return BrainTree::Node::Status::Failure;
	}
	
}