#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CAnimObject abstract : public CGameObject
{
protected:
	CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimObject(const CAnimObject& rhs);
	virtual ~CAnimObject() = default;

public:
	virtual void On_Collision(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag); // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit);
	virtual void On_CollisionExit(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag);

protected:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };

};

END

