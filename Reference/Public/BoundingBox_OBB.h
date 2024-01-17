#pragma once

#include "BoundParent.h"

BEGIN(Engine)

class CBoundingBox_OBB final : public CBoundParent
{
public:
	typedef struct : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;
private:
	CBoundingBox_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBoundingBox_OBB() = default;

public:
	BoundingOrientedBox* Get_Bounding() {return m_pBoundingOriented; }

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) override;
#endif // __DEBUG

	virtual void Update(_fmatrix TransformMatrix);

public:
	virtual _bool Collision(class CCollider* pTargetCollider, _bool* pisCollision) override;

private:
	BoundingOrientedBox*			m_pOriginBoundingOriented	= { nullptr };
	BoundingOrientedBox*			m_pBoundingOriented			= { nullptr };



public:
	static CBoundingBox_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END