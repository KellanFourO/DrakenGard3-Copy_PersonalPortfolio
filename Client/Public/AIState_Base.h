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
class CGameInstance;
END

BEGIN(Client)

class CAIState_Base abstract : public BrainTree::Node
{

public:
	BrainTree::Node::Status update() override;

protected:
	_bool		Detect();
	_bool		Chase();


protected:
	_float	fConsoleDebugAcc = 0.f;

};

END

