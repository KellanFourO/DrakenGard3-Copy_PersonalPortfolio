#include "EN00State_Idle.h"
#include "GameInstance.h"
#include "Animation.h"

//!m_pOwnerModelCom->Set_Animation(0);
//! m_fSearchRange = 10.f;
//!m_fAttackRange = 10.f;
//!m_pOwner = pOwner;
//!
//!m_pOwnerTransform->Set_SpeedPerSec(3.f);
//!m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(70.0f));


BrainTree::Node::Status CEN00State_Idle::update()
{
	fConsoleDebugAcc += blackboard->GetTimeDelta();
	
	
	SetAnimation(0);


	if (fConsoleDebugAcc > 2.f)
	{
		cout << "EN00 - BrainTree - Idle " << endl;
		fConsoleDebugAcc = 0.f;
	}

	if (true == Detect(blackboard->GetTarget(), "Detect_Range"))
	{
		cout << "EN00 - BrainTree - Chase " << endl;
		
		blackboard->GetTransform()->Set_SpeedPerSec(3.f);
		blackboard->GetTransform()->Set_RotationPerSec(XMConvertToRadians(70.0f));

		return BrainTree::Node::Status::Success;
	}
	
	
}
