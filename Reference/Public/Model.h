#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; //! 애니메이션이 있는 모델과 없는 모델을 구분짓기 위한 열거체

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex); //! virtual은 의미가없다. 자기 자신이 가지고잇는 메시들을 렌더링 시키기위한 함수이다.

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

private:
	//TODO aiScene은 뭔데?
	//! Assimp라이브러리를 추가해서 사용할 수 있는 타입이다. 구조체이며 모델과 관련된 정보를 모두 가지고있다
	//! 즉, 밑에 ReadFile 함수를 호출해서 aiScene 구조체를 채워줬다면 로드는 거의 끝났다고 할 수있다
	const aiScene*			m_pAIScene = { nullptr };

	//! 임포터는 ReadFile을 가지고있는 함수이고 ReadFile은 리턴값으로 aiScene* 를 뱉는다.
	Assimp::Importer		m_Importer;

private:
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 }; //!  AIScene으로 부터 읽어들일 재질텍스처 개수
	
	//! #머터리얼구조체의_텍스처클래스를_사용하는이유 원래는 ShaderResourceView*를 들고있어도 되지만 그렇게하면 텍스처를 로드하는 기능을 다시 작성해줘야 한다. 텍스처로드기능은 이미 텍스쳐클래스에서 구현해놓았으니 텍스처클래스를 배열로 가지고있자.
	vector<MATERIAL_DESC>	m_Materials; //! 어심프라이브러리에서 제공하는 재질 텍스처 정보열거체의 개수(18)만큼의 텍스처클래스 배열 == MATERIAL_DESC

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const string& strModelFilePath); //!#모델텍스처로드

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

