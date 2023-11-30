#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const string& strModelFilePath);
	virtual HRESULT Initialize(void* pArg);

private:
	//TODO aiScene�� ����?
	//! Assimp���̺귯���� �߰��ؼ� ����� �� �ִ� Ÿ���̴�. ����ü�̸� �𵨰� ���õ� ������ ��� �������ִ�
	//! ��, �ؿ� ReadFile �Լ��� ȣ���ؼ� aiScene ����ü�� ä����ٸ� �ε�� ���� �����ٰ� �� ���ִ�
	const aiScene*			m_pAIScene = { nullptr };

	//! �����ʹ� ReadFile�� �������ִ� �Լ��̰� ReadFile�� ���ϰ����� aiScene* �� ��´�.
	Assimp::Importer		m_Importer;

private:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private:
	HRESULT	Ready_Meshes();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

