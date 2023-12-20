#pragma once
#include "Component.h"

BEGIN(Engine)
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; //! �ִϸ��̼��� �ִ� �𵨰� ���� ���� �������� ���� ����ü

	struct ModelData
	{
		wstring strBoneDataPath;
		wstring strMeshDataPath;
		wstring strMaterialDataPath;
		wstring strAnimationDataPath;
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_uint Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }
	_bool Get_ChangeAnim() { return m_bChangeAnim;}

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	void  Set_Animation(_uint iAnimIndex) 
	{ 
		if( m_iPrevAnimIndex != m_iCurrentAnimIndex) //! ���� �ε����� ���� �ִϸ��̼��� �ε����� �ٸ��ٸ� ������ ��������
		{	
			m_iPrevAnimIndex	= m_iCurrentAnimIndex;  // ! ���� �ε������� �ٲ�� �� �ִϸ��̼��� �ε����� �־�����
			m_iCurrentAnimIndex = iAnimIndex; // !���� �ִϸ��̼��� �ε����� �ٲ� �ε����� �־�����

			m_pPrevAnimation = m_Animations[m_iPrevAnimIndex];
			m_bChangeAnim = true;
		}
		else
			m_iCurrentAnimIndex = iAnimIndex;
	}

	void  Finished_ChangeAnim() { m_bChangeAnim = false;}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix);
	//HRESULT Initialize_PrototypeLoad(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex); //! virtual�� �ǹ̰�����. �ڱ� �ڽ��� �������մ� �޽õ��� ������ ��Ű������ �Լ��̴�.

public:
	void	Play_Animation(_float fTimeDelta, _bool isLoop = true);

public:
	//! ���̴��� ���� ������� Ư���Žÿ��� ������ �ִ� ������� ���� �Ŷ���ߴ�. �𵨿��� �����°��̾ƴ� �Ž� Ŭ������ ���� ���� Ŭ������ ���ļ� �����ִ� ������ �ϴ� ���̴�.
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	_bool	Compute_MousePos(RAY _Ray, _matrix _WorldMatrix);

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

	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 }; //! AIScene���� ���� �о���� �ִϸ��̼� ����
	_uint						m_iCurrentAnimIndex = { 0 }; //! ���� �ִϸ��̼��� �ε����� �̸� �˾Ƴ��ƾ� ����.
	_uint						m_iPrevAnimIndex = { 0 };
	
	_float						m_fTimeAcc = { 0.f };

	_bool						m_bChangeAnim = { false };

	vector<class CAnimation*>	m_Animations;

	CAnimation*			m_pPrevAnimation = { nullptr };

	ModelData					m_tDataFilePath;
public:
	typedef vector<class CBone*>	BONES;

private:
	HRESULT Read_BoneData(const wstring& strPath);
	HRESULT Read_MeshData(const wstring& strPath, _fmatrix PivotMatrix);
	HRESULT Read_MaterialData(wstring& strPath);
	HRESULT Read_AnimationData(const wstring& strPath);

	_uint	Get_BoneIndex(const char* szName);

	string	ConvertWstrToStr(const wstring& wstr);
	wstring	ConvertStrToWstr(const string& str);
	string	ModifyPath(const string& originalPath);
	string	ReplaceExtension(const string& originalPath, const string& newExtension);
	
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END


