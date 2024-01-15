#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollider;
class CGameObject;

union COLLIDER_ID
{
	struct
	{
		UINT Left_id;
		UINT Right_id;
	};
	ULONGLONG ID;
};

class CCollision_Manager final : public CBase
{
private:
			CCollision_Manager();
	virtual ~CCollision_Manager() = default;


public:
	void		Update_CollisionMgr(_uint iLevelIndex);
	HRESULT		Add_Check_CollisionGroup(const _tchar* LeftLayerTag, const _tchar* RightLayerTag);

public:
	void		Collision_GroupUpdate(const _tchar* LeftTag, const _tchar* RightTag, _uint iLevelIndex);
	bool		Is_Collision(CCollider* pLeftCol, CCollider* pRightCol, _float* fX, _float* fY, _float* fZ);
	void		Get_Min_Max(CCollider* Col, _vector& vMin, _vector& vMax);
	XMVECTOR	Get_Min_Vector(CCollider* Col);
	XMVECTOR	Get_Max_Vector(CCollider* Col);

	_float		Get_Min(_float fSour, _float fDest);
	_float		Get_Max(_float fSour, _float fDest);

	void Reset_CollisionGroup() {
		m_ColLayers.clear();
	}

	bool Intersects(CCollider* pLeftCol, CCollider* pRightCol, _float* fX, _float* fY, _float* fZ);

	vector<pair<const _tchar*, const _tchar*>> m_ColLayers;

	map<ULONGLONG, _bool> m_mapColInfo;

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CCollision_Manager* Create();
	virtual void Free(void) override;

};

END

//! 오브젝트들의 충돌체들을 관리한다.
//! 맵으로?. 키값으로 그룹, 밸류로 충돌체.  그룹은 플레이어, 몬스터 정도?

//! 


//! 충돌 검사를 할 그룹별로 나눈다.
