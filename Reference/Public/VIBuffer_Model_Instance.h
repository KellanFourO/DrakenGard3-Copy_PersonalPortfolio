#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CTexture;
class CMesh;
class CShader;
class CBone;

class ENGINE_DLL CVIBuffer_Model_Instance final : public CVIBuffer
{


public:
	typedef struct tagModelMaterial
	{
		CTexture* pTextures[AI_TEXTURE_TYPE_MAX];
	}MODEL_MATERIAL;

private:
	CVIBuffer_Model_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs);
	virtual ~CVIBuffer_Model_Instance() = default;


public:
	wstring Get_ModelTag() const { return m_strModelTag;}

public:
	//TODO override�� ���� �̾߱�
	//! �������̵带 ���̸� �θ�Ŭ������ �Լ��� ȣ�������ʰ�, �ڽ��� �� �Լ��� ȣ���϶�� ������ ��
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	HRESULT         Render_Mesh(_uint iMeshContainerIndex);

public:
	void			Init_Model(wstring& In_ModelTag, _fmatrix PivotMatrix);
	void			Init_Instance(_uint In_iNumInstance);

	HRESULT			Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT			Bind_SRV(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eActorType);
	
	void			Update(vector<INSTANCE_MESH_DESC>& In_ParticleDescs);

private:
	HRESULT			Create_Materials();
	HRESULT			Create_Meshs();
	HRESULT			Create_Bones();
	void			Reset_Model();

private:
	wstring&		SliceModelTag(wstring& strModelTag);
	string			ModifyPath(const string& originalPath);
	string			ReplaceExtension(const string& originalPath, const string& newExtension);
	string			ConvertWstrToStrInstance(const wstring& wstr);
	wstring			ConvertStrToWstrInstance(const string& str);

private:
	ID3D11Buffer*		m_pVBInstance = { nullptr };
	_uint				m_iInstanceStride = { 0 };
	_uint				m_iNumInstance = { 0 };
	_uint				m_iIndexCountPerInstance = { 0 };

	_uint				m_iMaterialIndex = { 0 };
	
	wstring					m_strModelTag = { TEXT("") };

	MODELDATA				m_tModelData = {};
	vector<CBone*>			m_Bones;
	vector<CMesh*>			m_Meshes;
	vector<MATERIAL_DESC>	m_Materials;
	_uint					m_iNumMeshes = {0};
	_uint					m_iNumMaterials {0};
	_float4x4				m_PivotMatrix;
	


public:
	static CVIBuffer_Model_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END