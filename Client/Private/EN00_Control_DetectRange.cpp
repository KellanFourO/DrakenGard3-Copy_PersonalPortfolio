#include "EN00_Control_DetectRange.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Control_DetectRange::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_DetectRange::Control_Function()
{
	CTransform* pTargetTransform = blackboard->GetTarget()->Get_Transform();
	CTransform* pOwnerTransform = blackboard->GetTransform();

	_vector vTargetPos, vOwnerPos;

	vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	_float3 vDistance;
	XMStoreFloat3(&vDistance, XMVector3Length(vTargetPos - vOwnerPos));

	if (XMVectorGetX(XMLoadFloat3(&vDistance)) < blackboard->getFloat("Detect_Range"))
	{
		m_bCheck = true;
	}
	else 
		m_bCheck = false;


	return m_bCheck;
}
