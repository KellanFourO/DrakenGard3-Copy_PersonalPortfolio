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
	char				m_szName[MAX_PATH]; //! 메쉬 이름 정보 받아주자
	_uint				m_iMaterialIndex = { 0 };

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;

	vector<_float3>		m_MeshVertexs;
	vector<_uint3>		m_MeshIndices;

public:
	//TODO Create시에 인자값으로 const aiMesh* 를 받는 것을 알 수 있다.
	//! aiMesh는 어심프 라이브러리로 읽어들인 aiScene 구조체 내부에 들어있다.
	//! 그러니 당연히 읽어들이면서 Mesh를 생성해준다는 느낌으로 가면 될것.
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXANIMMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<class CBone*> Bones);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, vector<VTXMESH>& Vertices, vector<_int>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END

