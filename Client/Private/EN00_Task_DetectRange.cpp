#include "stdafx.h"
#include "EN00_Task_DetectRange.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Task_DetectRange::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_DetectRange::Task()
{
    StartLoop();
    SetAnimation(3);

    //! 

    CTransform* pTargetTransform = blackboard->GetTarget()->Get_Transform();
    CTransform* pOwnerTransform = blackboard->GetTransform();
        
    pOwnerTransform->Set_SpeedPerSec(1.8f);
    pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.f));

    _vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
    _vector vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

    pOwnerTransform->Go_Target_Navi(vTargetPos, blackboard->GetTimeDelta(), blackboard->GetNavigation());

    _float3 vDistance;
    XMStoreFloat3(&vDistance, XMVector3Length(vTargetPos - vOwnerPos));
    
    if(!(XMVectorGetX(XMLoadFloat3(&vDistance)) < blackboard->getFloat("Detect_Range")))
    {
        return BrainTree::Node::Status::Success;
    }
    
        
    
}
