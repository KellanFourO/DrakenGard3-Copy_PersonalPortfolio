#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; //! �ִϸ��̼��� �ִ� �𵨰� ���� ���� �������� ���� ����ü

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex); //! virtual�� �ǹ̰�����. �ڱ� �ڽ��� �������մ� �޽õ��� ������ ��Ű������ �Լ��̴�.

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

private:
	//TODO aiScene�� ����?
	//! Assimp���̺귯���� �߰��ؼ� ����� �� �ִ� Ÿ���̴�. ����ü�̸� �𵨰� ���õ� ������ ��� �������ִ�
	//! ��, �ؿ� ReadFile �Լ��� ȣ���ؼ� aiScene ����ü�� ä����ٸ� �ε�� ���� �����ٰ� �� ���ִ�
	const aiScene*			m_pAIScene = { nullptr };

	//! �����ʹ� ReadFile�� �������ִ� �Լ��̰� ReadFile�� ���ϰ����� aiScene* �� ��´�.
	Assimp::Importer		m_Importer;

private:
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 }; //!  AIScene���� ���� �о���� �����ؽ�ó ����
	
	//! #���͸�����ü��_�ؽ�óŬ������_����ϴ����� ������ ShaderResourceView*�� ����־ ������ �׷����ϸ� �ؽ�ó�� �ε��ϴ� ����� �ٽ� �ۼ������ �Ѵ�. �ؽ�ó�ε����� �̹� �ؽ���Ŭ�������� �����س������� �ؽ�óŬ������ �迭�� ����������.
	vector<MATERIAL_DESC>	m_Materials; //! ��������̺귯������ �����ϴ� ���� �ؽ�ó ��������ü�� ����(18)��ŭ�� �ؽ�óŬ���� �迭 == MATERIAL_DESC

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const string& strModelFilePath); //!#���ؽ�ó�ε�

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

