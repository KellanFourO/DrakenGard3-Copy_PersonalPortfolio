#pragma once
#include "Client_Defines.h"
#include "BrainTree/BrainTree.h"
#include <iostream>

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTransform;
class CRigidBody;
class CGameObject;
class CGameInstance;

END

BEGIN(Client)

class CAIState_Base abstract : public BrainTree::Node
{

public:
	BrainTree::Node::Status update() override;

protected:
	void		SetAnimation(_int iAnimIndex);
	_bool		Detect(CGameObject* pTarget, const string& strDetectRangeKey);
	_vector		Chase(CGameObject* pTarget);
	_bool		AttackEnable(_float3 vDistance, const string& strAttackRangeKey);
	void		KeepEye(CGameObject* pTarget);


protected:
	_float	fConsoleDebugAcc = 0.f;
	_float3	m_vDistance = { 0.f, 0.f, 0.f };

};

END

