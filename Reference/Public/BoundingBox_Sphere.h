#pragma once

#include "BoundParent.h"

BEGIN(Engine)

class CBoundingBox_Sphere final : public CBoundParent
{
public:
	typedef struct : public BOUNDING_DESC
	{
		_float		fRadius;
	}BOUNDING_SPHERE_DESC;
private:
	CBoundingBox_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBoundingBox_Sphere() = default;

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);

#ifdef _DEBUG	
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingSphere*		m_pOriginBoundingSphere		= { nullptr };
	BoundingSphere*		m_pBoundingSphere			= { nullptr };

public:
	static CBoundingBox_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END