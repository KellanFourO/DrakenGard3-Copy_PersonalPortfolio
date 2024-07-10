#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CPartObject;
//struct MODEL_DATA;

class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	typedef struct tag_TrailBuffer
	{
		_float3		vStartPos;
		_float3		vEndPos;
		_uint		iMaxCount;
		
	}TRAIL_DESC;

protected:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_float fTimeDelta, _fmatrix OwnerWorldMatrix);
	//void Update(_float _fTimeDelta, class CTransform* _pOwnerTransform, class CBone* _pOwnerBoneNode);
	void Tick(_float fTimeDelta);
	
//public:
	void Reset_Points(_fmatrix OwnerWorldMatrix);

protected:
	_bool m_bTrailOn;
	_uint m_iVertexCount;
	_uint m_iNumVertices;
	_float4 m_vLocalSwordLow;
	_float4 m_vLocalSwordHigh;
	TRAIL_DESC m_tTrailDesc;
	_uint m_iLerpPointNum = 12;
	_int m_iCatMullRomIndex[4];

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END