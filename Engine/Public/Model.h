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
	_uint Get_Animation() { return m_iCurrentAnimIndex; }

	void  Set_Animation(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex); //! virtual�� �ǹ̰�����. �ڱ� �ڽ��� �������մ� �޽õ��� ������ ��Ű������ �Լ��̴�.

public:
	void	Play_Animation(_float fTimeDelta, _bool isLoop = true);

public:
	//! ���̴��� ���� ������� Ư���Žÿ��� ������ �ִ� ������� ���� �Ŷ���ߴ�. �𵨿��� �����°��̾ƴ� �Ž� Ŭ������ ���� ���� Ŭ������ ���ļ� �����ִ� ������ �ϴ� ���̴�.
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

private:
	//TODO aiScene�� ����?
	//! Assimp���̺귯���� �߰��ؼ� ����� �� �ִ� Ÿ���̴�. ����ü�̸� �𵨰� ���õ� ������ ��� �������ִ�
	//! ��, �ؿ� ReadFile �Լ��� ȣ���ؼ� aiScene ����ü�� ä����ٸ� �ε�� ���� �����ٰ� �� ���ִ�
	const aiScene*			m_pAIScene = { nullptr };

	//! �����ʹ� ReadFile�� �������ִ� �Լ��̰� ReadFile�� ���ϰ����� aiScene* �� ��´�.
	Assimp::Importer		m_Importer;

private:
	//! ������� ������ �ִԸ��� �츮�� ���ϴ� ���� �Ǵ� ũ��� �����س��� �;��� �� ������ ����� ��������� �а�.
	_float4x4				m_PivotMatrix; 

	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 }; //!  AIScene���� ���� �о���� �����ؽ�ó ����
	
	//! #���͸�����ü��_�ؽ�óŬ������_����ϴ����� ������ ShaderResourceView*�� ����־ ������ �׷����ϸ� �ؽ�ó�� �ε��ϴ� ����� �ٽ� �ۼ������ �Ѵ�. �ؽ�ó�ε����� �̹� �ؽ���Ŭ�������� �����س������� �ؽ�óŬ������ �迭�� ����������.
	vector<MATERIAL_DESC>	m_Materials; //! ��������̺귯������ �����ϴ� ���� �ؽ�ó ��������ü�� ����(18)��ŭ�� �ؽ�óŬ���� �迭 == MATERIAL_DESC

	vector<class CBone*>	m_Bones;

	_uint						m_iNumAnimations = { 0 }; //! AIScene���� ���� �о���� �ִϸ��̼� ����
	_uint						m_iCurrentAnimIndex = { 0 }; //! ���� �ִϸ��̼��� �ε����� �̸� �˾Ƴ��ƾ� ����.
	vector<class CAnimation*>	m_Animations;
public:
	typedef vector<class CBone*>	BONES;

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const string& strModelFilePath); //!#���ؽ�ó�ε�
	HRESULT	Ready_Bones(aiNode* pAInode, _int iParentIndex); //! _uint�� �ƴ� _int�� ����� ������ �θ� �������� �ʴ� ����ϰ�쿡�� -1�� �����ϱ� ���ؼ��̴�.
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

