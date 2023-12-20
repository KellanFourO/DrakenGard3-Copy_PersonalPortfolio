#pragma once
#include "Component.h"

BEGIN(Engine)
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END }; //! 애니메이션이 있는 모델과 없는 모델을 구분짓기 위한 열거체

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
		if( m_iPrevAnimIndex != m_iCurrentAnimIndex) //! 이전 인덱스와 현재 애니메이션의 인덱스가 다르다면 보간을 시작하자
		{	
			m_iPrevAnimIndex	= m_iCurrentAnimIndex;  // ! 이전 인덱스에게 바뀌기 전 애니메이션의 인덱스를 넣어주자
			m_iCurrentAnimIndex = iAnimIndex; // !현재 애니메이션의 인덱스를 바꾼 인덱스를 넣어주자

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
	virtual HRESULT Render(_uint iMeshIndex); //! virtual은 의미가없다. 자기 자신이 가지고잇는 메시들을 렌더링 시키기위한 함수이다.

public:
	void	Play_Animation(_float fTimeDelta, _bool isLoop = true);

public:
	//! 셰이더에 던질 뼈행렬은 특정매시에게 영향을 주는 뼈행렬을 던질 거라고했다. 모델에서 던지는것이아닌 매쉬 클래스를 통해 현재 클래스를 거쳐서 던져주는 행위를 하는 것이다.
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	_bool	Compute_MousePos(RAY _Ray, _matrix _WorldMatrix);

private:
	//TODO aiScene은 뭔데?
	//! Assimp라이브러리를 추가해서 사용할 수 있는 타입이다. 구조체이며 모델과 관련된 정보를 모두 가지고있다
	//! 즉, 밑에 ReadFile 함수를 호출해서 aiScene 구조체를 채워줬다면 로드는 거의 끝났다고 할 수있다
	const aiScene*			m_pAIScene = { nullptr };

	//! 임포터는 ReadFile을 가지고있는 함수이고 ReadFile은 리턴값으로 aiScene* 를 뱉는다.
	Assimp::Importer		m_Importer;

private:
	//! 뼈행렬이 곱해진 애님모델을 우리가 원하는 방향 또는 크기로 셋팅해놓고 싶었고 그 곱해줄 행렬을 멤버변수로 둔것.
	_float4x4				m_PivotMatrix; 

	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 }; //!  AIScene으로 부터 읽어들일 재질텍스처 개수
	
	//! #머터리얼구조체의_텍스처클래스를_사용하는이유 원래는 ShaderResourceView*를 들고있어도 되지만 그렇게하면 텍스처를 로드하는 기능을 다시 작성해줘야 한다. 텍스처로드기능은 이미 텍스쳐클래스에서 구현해놓았으니 텍스처클래스를 배열로 가지고있자.
	vector<MATERIAL_DESC>	m_Materials; //! 어심프라이브러리에서 제공하는 재질 텍스처 정보열거체의 개수(18)만큼의 텍스처클래스 배열 == MATERIAL_DESC

	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 }; //! AIScene으로 부터 읽어들일 애니메이션 개수
	_uint						m_iCurrentAnimIndex = { 0 }; //! 현재 애니메이션의 인덱스를 미리 알아놓아야 편해.
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


