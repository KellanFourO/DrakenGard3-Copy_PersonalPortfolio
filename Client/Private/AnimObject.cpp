#include "stdafx.h"
#include "AnimObject.h"

CAnimObject::CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAnimObject::CAnimObject(const CAnimObject& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

void CAnimObject::On_Collision(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag)
{
}

void CAnimObject::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
}

void CAnimObject::On_CollisionExit(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag)
{
}
