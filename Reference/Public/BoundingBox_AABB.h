#pragma once

#include "BoundParent.h"

BEGIN(Engine)

class CBoundingBox_AABB final : public CBoundParent
{
public:
	typedef struct : public BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBoundingBox_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBoundingBox_AABB() = default;

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);

#ifdef _DEBUG	
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingBox*			m_pOriginBoundingBox	= { nullptr };
	BoundingBox*			m_pBoundingBox			= { nullptr };

public:
	static CBoundingBox_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END