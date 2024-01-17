#include "stdafx.h"
#include "EN00_Control_KeepEye.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Control_KeepEye::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_KeepEye::Control_Function()
{

	_int iRandomNumber = Random({0,1,0,1,1,0,1,0,1});

	CTransform* pTargetTransform = blackboard->GetTarget()->Get_Transform();
	CTransform* pOwnerTransform = blackboard->GetTransform();

	_vector vTargetPos, vOwnerPos;

	vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	_float3 vDistance;
	XMStoreFloat3(&vDistance, XMVector3Length(vTargetPos - vOwnerPos));

	if (XMVectorGetX(XMLoadFloat3(&vDistance)) < blackboard->getFloat("Attack_Range"))
	{
		switch (iRandomNumber)
		{
		case 0:
		{
			pOwnerTransform->Look_At(vTargetPos);
			m_bCheck = true;
			break;
		}
			

		case 1:
		{
			pOwnerTransform->Look_At(vTargetPos);
			m_bCheck = false;
			break;
		}
			
		}
	}
	else
		m_bCheck = false;




	
	return m_bCheck;
}
