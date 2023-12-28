#include "stdafx.h"
#include "AIState_Base.h"
#include "Transform.h"
#include "GameObject.h"
#include "Animation.h"

BrainTree::Node::Status CAIState_Base::update()
{
	cout << "AIState_Base" << endl;
	
	return BrainTree::Node::Status();
}

void CAIState_Base::SetAnimation(_int iAnimIndex)
{
	if (iAnimIndex != blackboard->GetModel()->Get_CurrentAnimationIndex())
		blackboard->GetModel()->Set_Animation(iAnimIndex);
}

_bool CAIState_Base::Detect(CGameObject* pTarget, const string& strDetectRangeKey)
{
	
	CTransform* pOwnerTransform = blackboard->GetTransform();


 	_vector vTargetPos, vOwnerPos;

	vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
 	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);
 	
	

	_vector vDistance = XMVector3Length(vTargetPos - vOwnerPos);
 
 	if (XMVectorGetX(vDistance) < blackboard->getFloat(strDetectRangeKey))
 	{
 		return true;
 	}

	return false;
}

_vector CAIState_Base::Chase(CGameObject* pTarget)
{
	CTransform* pOwnerTransform = blackboard->GetTransform();
	
	_vector vTargetPos, vOwnerPos;

	vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);
   	
	pOwnerTransform->Go_Target(vTargetPos, blackboard->GetTimeDelta());
 
 	_vector vDistance = XMVector3Length(vTargetPos - vOwnerPos);
 
	return vDistance;

 	
}

_bool CAIState_Base::AttackEnable(_float3 vDistance, const string& strAttackRangeKey)
{
	if (XMVectorGetX(XMLoadFloat3(&vDistance)) < blackboard->getFloat(strAttackRangeKey))
	{
		return true;
	}

	return false;
}
