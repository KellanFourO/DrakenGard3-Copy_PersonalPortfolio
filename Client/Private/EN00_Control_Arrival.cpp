#include "EN00_Control_Arrival.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Control_Arrival::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_Arrival::Control_Function()
{
	if(true == blackboard->getBool("Is_TargetPosition"))
		m_bCheck = true;

	else
		m_bCheck = false;

	if (true == m_bCheck)
	{
		CTransform* pOwnerTransform = blackboard->GetTransform();

		_float3 vCurrentPos;
		XMStoreFloat3(&vCurrentPos, pOwnerTransform->Get_State(CTransform::STATE_POSITION));

		_float3 vTargetPos = blackboard->getFloat3("TargetPostion");

		return pOwnerTransform->HasArrived(vCurrentPos, vTargetPos, 5.f);
	}
	else
		return false;
}
