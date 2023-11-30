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
	//TODO aiScene은 뭔데?
	//! Assimp라이브러리를 추가해서 사용할 수 있는 타입이다. 구조체이며 모델과 관련된 정보를 모두 가지고있다
	//! 즉, 밑에 ReadFile 함수를 호출해서 aiScene 구조체를 채워줬다면 로드는 거의 끝났다고 할 수있다
	const aiScene*			m_pAIScene = { nullptr };

	//! 임포터는 ReadFile을 가지고있는 함수이고 ReadFile은 리턴값으로 aiScene* 를 뱉는다.
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

