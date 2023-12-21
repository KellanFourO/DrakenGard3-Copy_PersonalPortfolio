#pragma once

#include "Base.h"

#ifdef _DEBUG
#include "./DebugDraw/DebugDraw.h"
#endif

BEGIN(Engine)

class CBoundParent abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;

protected:
	CBoundParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBoundParent() = default;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
#endif


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };


public:	
	virtual void Free() override;
};

END