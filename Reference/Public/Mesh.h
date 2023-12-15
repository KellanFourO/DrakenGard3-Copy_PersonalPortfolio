#pragma once
#include "VIBuffer.h"
#include "Model.h"


BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<class CBone*> Bones);
	virtual HRESULT Initialize_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);
	_bool	Compute_MousePos(RAY _Ray, _matrix _WorldMatrix);
	
private:
	HRESULT Ready_Vertices_NonAnim(vector<VTXMESH>& Vertices, _fmatrix PivotMatrix);
	HRESULT Ready_Vertices_Anim(vector<VTXANIMMESH>& Vertices, vector<class CBone*> Bones);

private:
	char				m_szName[MAX_PATH]; //! �޽� �̸� ���� �޾�����
	_uint				m_iMaterialIndex = { 0 };

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;

	vector<_float3>		m_MeshVertexs;
	vector<_uint3>		m_MeshIndices;

public:
	//TODO Create�ÿ� ���ڰ����� const aiMesh* �� �޴� ���� �� �� �ִ�.
	//! aiMesh�� ����� ���̺귯���� �о���� aiScene ����ü ���ο� ����ִ�.
	//! �׷��� �翬�� �о���̸鼭 Mesh�� �������شٴ� �������� ���� �ɰ�.
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<class CBone*> Bones);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END

