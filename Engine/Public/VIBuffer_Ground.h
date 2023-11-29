#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Ground final : public CVIBuffer
{
private:
	CVIBuffer_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Ground(const CVIBuffer_Ground& rhs);
	virtual ~CVIBuffer_Ground() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_bool			Get_Vertex(_uint _iIndex, VTXGROUND* _pOut);
	_bool			Get_Indices(_uint _iIndex, _uint3* _pOut);
	_uint			Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint			Get_NumVerticesZ() { return m_iNumVerticesZ; }
	_float			Get_Interval() { return m_fInterval; }

public:
	void			Update(_vector _vMousePos, _float _fRadious, _float _fPower, _uint _iMode);
	_bool			Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut);
	_bool			Compute_MouseRatio(RAY _Ray, _matrix _WorldMatrix, _float2* pOut);
	_bool			Compute_IsInTerrain(_uint _iNumVertexX, _uint _iNumVertexZ, _float _fInterval, _fvector _vTargetPos, _fmatrix _WorldMatrix);

private:
	_uint					m_iNumVerticesX = 0, m_iNumVerticesZ = 0;
	_float					m_fInterval = 0.5f;

	vector<_uint3>			m_Indices;
	vector<VTXGROUND>		m_VertexInfo;

public:
	static	CVIBuffer_Ground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END