#include "Collision_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

#include "BoundingBox_AABB.h"
#include "BoundingBox_OBB.h"
#include "BoundingBox_Sphere.h"
#include "PartObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CCollision_Manager::Update_CollisionMgr(_uint iLevelIndex, _float fTimeDelta)
{
	for (auto& pair : m_ColLayers)
	{
		Collision_GroupUpdate(pair.first, pair.second, iLevelIndex, fTimeDelta);
	}

}

HRESULT CCollision_Manager::Add_Check_CollisionGroup(const _tchar* LeftLayerTag, const _tchar* RightLayerTag)
{
	for (auto& pair : m_ColLayers)
	{
		if ((!lstrcmp(pair.first, LeftLayerTag) && !lstrcmp(pair.second, RightLayerTag))
			|| (!lstrcmp(pair.second, LeftLayerTag) && !lstrcmp(pair.first, RightLayerTag)))
			return E_FAIL;
	}

	m_ColLayers.push_back({ LeftLayerTag, RightLayerTag });

	return S_OK;
}

void CCollision_Manager::Collision_GroupUpdate(const _tchar* LeftTag, const _tchar* RightTag, _uint iLevelIndex, _float fTimeDelta)
{
	//! 레프트 레이어태그가 플레이어라면
	//! 레프트 콜라이더는  웨폰.
	//! 라이트 콜라이더는 바디.

	

	wstring strLeftTag = LeftTag;
	wstring strRightTag = RightTag;

	CLayer* pLeftLayer = m_pGameInstance->Find_Layer(iLevelIndex, LeftTag);
	CLayer* pRightLayer = m_pGameInstance->Find_Layer(iLevelIndex, RightTag);

	list<CGameObject*> pLeftObjectList = pLeftLayer->Get_ObjectList();
	list<CGameObject*> pRightObjectList = pRightLayer->Get_ObjectList();



	map<ULONGLONG, _bool>::iterator iter;

	CCollider* pLeftCol = nullptr;

	for (auto& pLeftGameObject : pLeftObjectList)
	{
		pLeftCol = pLeftGameObject->Find_Collider(true);

		if (false == pLeftCol->isOnCollider())
			continue;

		for (auto& pRightGameObject : pRightObjectList)
		{
			if(pLeftGameObject == pRightGameObject)
				continue;

			CCollider* pRightCol = pRightGameObject->Find_Collider(false);

			COLLIDER_ID ID;

			ID.Left_id = pLeftCol->Get_ID();
			ID.Right_id = pRightCol->Get_ID();

			iter = m_mapColInfo.find(ID.ID);

			if (m_mapColInfo.end() == iter)
			{
				m_mapColInfo.insert({ ID.ID, false });
				iter = m_mapColInfo.find(ID.ID);
			}

			_float fX(0.f), fY(0.f), fZ(0.f);

			
			

			if (true == Is_Collision(pLeftCol, pRightCol, &fX, &fY, &fZ))
			{
				if (iter->second) // 충돌했고 이미 충돌 true인 경우
				{
					if (pLeftGameObject->Is_Dead() || pRightGameObject->Is_Dead())
					{
						
						pLeftGameObject->On_CollisionExit(pLeftGameObject, strLeftTag, pRightGameObject, strRightTag);

						pLeftCol->On_CollisionExit(pRightCol, fX, fY, fZ);
						pRightCol->On_CollisionExit(pLeftCol, fX, fY, fZ);
						iter->second = false;
					}

					else
					{
						if (true == pLeftCol->isAccCollider())
						{
							_float fHitTick = pLeftCol->Get_HitTick();
							m_fAccTime += fTimeDelta;

							if (m_fAccTime >= fHitTick)
							{
								pLeftGameObject->On_Collision(pLeftGameObject, strLeftTag, pRightGameObject, strRightTag);
								
								pLeftCol->On_Collision(pRightCol, fX, fY, fZ);
								pRightCol->On_Collision(pLeftCol, fX, fY, fZ);

								
								m_fAccTime = 0.f;
							}
						}
						else
						{
							pLeftGameObject->On_Collision(pLeftGameObject, strLeftTag, pRightGameObject, strRightTag);
							pRightCol->OffCollider();
							pLeftCol->On_Collision(pRightCol, fX, fY, fZ);
							pRightCol->On_Collision(pLeftCol, fX, fY, fZ);
						}
					}
				}

				else // 충돌했는데 충돌 정보에 false로 기록된 경우
				{
					if (!pLeftGameObject->Is_Dead() && !pRightGameObject->Is_Dead() && true == pRightCol->isOnCollider())
					{
						pLeftGameObject->On_CollisionEnter(pLeftGameObject, strLeftTag, pRightGameObject, strRightTag);
						
						pLeftCol->On_CollisionEnter(pRightCol, fX, fY, fZ);
						pRightCol->On_CollisionEnter(pLeftCol, fX, fY, fZ);
						

						iter->second = true;
					}
				}
			}

			else // 충돌하지 않은 경우
			{
				if (iter->second)
				{
					pLeftGameObject->On_CollisionExit(pLeftGameObject, strLeftTag, pRightGameObject, strRightTag);
					
					pLeftCol->On_CollisionExit(pRightCol, fX, fY, fZ);
					pRightCol->On_CollisionExit(pLeftCol, fX, fY, fZ);
					pRightCol->OnCollider();
					
					iter->second = false;

				}
			}
		}
	}
	
	//! 플레이어의 충돌체가 On이 되었고. 피격대상인 모든 RightObject와 충돌검사를 한 뒤에 플레이어의 충돌체를 꺼준다.
	
	
}

bool CCollision_Manager::Is_Collision(CCollider* pLeftCol, CCollider* pRightCol, _float* fX, _float* fY, _float* fZ)
{
	_bool result = pLeftCol->Collision(pRightCol);

	//if (pLeftCol->Get_Type() == CCollider::TYPE_SPHERE || pRightCol->Get_Type() == CCollider::TYPE_SPHERE)
	//	return result;

	if (result && (pLeftCol->Get_ColliderType() == CCollider::TYPE_AABB || pLeftCol->Get_ColliderType() == CCollider::TYPE_OBB || pRightCol->Get_ColliderType() == CCollider::TYPE_AABB || pRightCol->Get_ColliderType() == CCollider::TYPE_OBB))
	{
		// 1차 충돌 검사 - AABB
		_vector vLeftMin = Get_Min_Vector(pLeftCol);
		_vector vRightMin = Get_Min_Vector(pRightCol);

		_vector vLeftMax = Get_Max_Vector(pLeftCol);
		_vector vRightMax = Get_Max_Vector(pRightCol);

		if (result)
			Intersects(pLeftCol, pRightCol, fX, fY, fZ);

		/*if (Get_Max(XMVectorGetX(vLeftMin), XMVectorGetX(vRightMin)) > Get_Min(XMVectorGetX(vLeftMax), XMVectorGetX(vRightMax)))
			return false;

		if (Get_Max(XMVectorGetY(vLeftMin), XMVectorGetY(vRightMin)) > Get_Min(XMVectorGetY(vLeftMax), XMVectorGetY(vRightMax)))
			return false;

		if (Get_Max(XMVectorGetZ(vLeftMin), XMVectorGetZ(vRightMin)) > Get_Min(XMVectorGetZ(vLeftMax), XMVectorGetZ(vRightMax)))
			return false;*/

		// 밀어낼 정도를 계산하여 반환
		/*fX = Get_Min(XMVectorGetX(vLeftMax), XMVectorGetX(vRightMax) - Get_Max(XMVectorGetX(vLeftMin), XMVectorGetX(vRightMin)));
		*fY = Get_Min(XMVectorGetY(vLeftMax), XMVectorGetY(vRightMax) - Get_Max(XMVectorGetY(vLeftMin), XMVectorGetY(vRightMin)));
		*fZ = Get_Min(XMVectorGetZ(vLeftMax), XMVectorGetZ(vRightMax) - Get_Max(XMVectorGetZ(vLeftMin), XMVectorGetZ(vRightMin)));*/
	}

	return result;
}

void CCollision_Manager::Get_Min_Max(CCollider* Col, _vector& vMin, _vector& vMax)
{
	XMVECTOR Min{}, Max{}, Corner{};

	if (nullptr == Col)
		return;

	BoundingBox* pAABB = dynamic_cast<CBoundingBox_AABB*>(Col->Get_Bounding())->Get_Bounding();
	BoundingOrientedBox* pOBB = dynamic_cast<CBoundingBox_OBB*>(Col->Get_Bounding())->Get_Bounding();

	if (Col->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		for (size_t i = 1; i < pAABB->CORNER_COUNT; ++i)
		{
			vMin = XMVectorMin(vMin, Corner);
			vMax = XMVectorMax(vMax, Corner);
		}
	}

	if (Col->Get_ColliderType() == CCollider::TYPE_OBB)
	{
		for (size_t i = 1; i < pOBB->CORNER_COUNT; ++i)
		{
			vMin = XMVectorMin(vMin, Corner);
			vMax = XMVectorMax(vMax, Corner);
		}
	}
}

XMVECTOR CCollision_Manager::Get_Min_Vector(CCollider* Col)
{
	XMVECTOR Min = {  };

	if (nullptr == Col)
		return Min;

	

	
	

	if (Col->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		BoundingBox* pAABB = dynamic_cast<CBoundingBox_AABB*>(Col->Get_Bounding())->Get_Bounding();

		_float3 coners[8];
		pAABB->GetCorners(coners);
		for (size_t i = 0; i < pAABB->CORNER_COUNT; ++i)
		{
			Min = XMVectorMin(Min, XMLoadFloat3(&coners[i]));
		}
	}

	else if (Col->Get_ColliderType() == CCollider::TYPE_OBB)
	{
		BoundingOrientedBox* pOBB = dynamic_cast<CBoundingBox_OBB*>(Col->Get_Bounding())->Get_Bounding();

		_float3 coners[8];
		pOBB->GetCorners(coners);
		for (size_t i = 0; i < pOBB->CORNER_COUNT; ++i)
		{
			Min = XMVectorMin(Min, XMLoadFloat3(&coners[i]));
		}
	}

	return Min;
}

XMVECTOR CCollision_Manager::Get_Max_Vector(CCollider* Col)
{
	XMVECTOR Max = {  };

	if (nullptr == Col)
		return Max;

	if (Col->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		BoundingBox* pAABB = dynamic_cast<CBoundingBox_AABB*>(Col->Get_Bounding())->Get_Bounding();

		_float3 coners[8];
		pAABB->GetCorners(coners);
		for (size_t i = 0; i < pAABB->CORNER_COUNT; ++i)
		{
			Max = XMVectorMax(Max, XMLoadFloat3(&coners[i]));
		}
	}

	else if (Col->Get_ColliderType() == CCollider::TYPE_OBB)
	{
		BoundingOrientedBox* pOBB = dynamic_cast<CBoundingBox_OBB*>(Col->Get_Bounding())->Get_Bounding();

		_float3 coners[8];
		pOBB->GetCorners(coners);
		for (size_t i = 0; i < pOBB->CORNER_COUNT; ++i)
		{
			Max = XMVectorMax(Max, XMLoadFloat3(&coners[i]));
		}
	}

	return Max;
}

_float CCollision_Manager::Get_Min(_float fSour, _float fDest)
{
	_float fMin = fSour < fDest ? fSour : fDest;

	return fMin;
}

_float CCollision_Manager::Get_Max(_float fSour, _float fDest)
{
	_float fMax = fSour > fDest ? fSour : fDest;

	return fMax;
}

bool CCollision_Manager::Intersects(CCollider* pLeftCol, CCollider* pRightCol, _float* fX, _float* fY, _float* fZ)
{
	XMVECTOR CenterA = {};
	XMVECTOR ExtentsA = {};
	XMVECTOR CenterB = {};
	XMVECTOR ExtentsB = {};

	BoundingBox* pLeftAABB = nullptr;
	BoundingBox* pRightAABB = nullptr;
	BoundingOrientedBox* pLeftOBB = nullptr;
	BoundingOrientedBox* pRightOBB = nullptr;

	if (pLeftCol->Get_ColliderType() == CCollider::TYPE_AABB && pRightCol->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		pLeftAABB = dynamic_cast<CBoundingBox_AABB*>(pLeftCol->Get_Bounding())->Get_Bounding();
		pRightAABB = dynamic_cast<CBoundingBox_AABB*>(pRightCol->Get_Bounding())->Get_Bounding();

		CenterA = XMLoadFloat3(&pLeftAABB->Center);

		CenterA = XMLoadFloat3(&pLeftAABB->Center);
		ExtentsA = XMLoadFloat3(&pLeftAABB->Extents);

		CenterB = XMLoadFloat3(&pRightAABB->Center);
		ExtentsB = XMLoadFloat3(&pRightAABB->Extents);
	}

	else if (pLeftCol->Get_ColliderType() == CCollider::TYPE_AABB && pRightCol->Get_ColliderType() == CCollider::TYPE_OBB)
	{
		pLeftAABB = dynamic_cast<CBoundingBox_AABB*>(pLeftCol->Get_Bounding())->Get_Bounding();
		pRightOBB = dynamic_cast<CBoundingBox_OBB*>(pRightCol->Get_Bounding())->Get_Bounding();

		CenterA = XMLoadFloat3(&pLeftAABB->Center);
		ExtentsA = XMLoadFloat3(&pLeftAABB->Extents);

		CenterB = XMLoadFloat3(&pRightOBB->Center);
		ExtentsB = XMLoadFloat3(&pRightOBB->Extents);
	}

	else if (pLeftCol->Get_ColliderType() == CCollider::TYPE_OBB && pRightCol->Get_ColliderType() == CCollider::TYPE_AABB)
	{
		pLeftOBB = dynamic_cast<CBoundingBox_OBB*>(pLeftCol->Get_Bounding())->Get_Bounding();
		pRightAABB = dynamic_cast<CBoundingBox_AABB*>(pRightCol->Get_Bounding())->Get_Bounding();

		CenterA = XMLoadFloat3(&pLeftOBB->Center);
		ExtentsA = XMLoadFloat3(&pLeftOBB->Extents);

		CenterB = XMLoadFloat3(&pRightAABB->Center);
		ExtentsB = XMLoadFloat3(&pRightAABB->Extents);
	}

	else if (pLeftCol->Get_ColliderType() == CCollider::TYPE_OBB && pRightCol->Get_ColliderType() == CCollider::TYPE_OBB)
	{
		pLeftOBB = dynamic_cast<CBoundingBox_OBB*>(pLeftCol->Get_Bounding())->Get_Bounding();
		pRightOBB = dynamic_cast<CBoundingBox_OBB*>(pRightCol->Get_Bounding())->Get_Bounding();

		CenterA = XMLoadFloat3(&pLeftOBB->Center);
		ExtentsA = XMLoadFloat3(&pLeftOBB->Extents);

		CenterB = XMLoadFloat3(&pRightOBB->Center);
		ExtentsB = XMLoadFloat3(&pRightOBB->Extents);
	}

	XMVECTOR MinA = XMVectorSubtract(CenterA, ExtentsA);
	XMVECTOR MaxA = XMVectorAdd(CenterA, ExtentsA);

	XMVECTOR MinB = XMVectorSubtract(CenterB, ExtentsB);
	XMVECTOR MaxB = XMVectorAdd(CenterB, ExtentsB);

	*fX = Get_Min(XMVectorGetX(MaxA), XMVectorGetX(MaxB)) - Get_Max(XMVectorGetX(MinA), XMVectorGetX(MinB));
	*fY = Get_Min(XMVectorGetY(MaxA), XMVectorGetY(MaxB)) - Get_Max(XMVectorGetY(MinA), XMVectorGetY(MinB));
	*fZ = Get_Min(XMVectorGetZ(MaxA), XMVectorGetZ(MaxB)) - Get_Max(XMVectorGetZ(MinA), XMVectorGetZ(MinB));

	return true;
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	return pInstance;
}

void CCollision_Manager::Free(void)
{
	Safe_Release(m_pGameInstance);
	m_ColLayers.clear();
	m_mapColInfo.clear();
}
